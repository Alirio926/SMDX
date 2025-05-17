import java.io.*;
import java.nio.file.*;
import java.util.*;

public class TMXObjectParserSplitOutput {

    static class TMXObject {
        String name;
        int x = -1;
        int y = -1;
        Map<String, String> properties = new LinkedHashMap<>();
        Map<String, String> propertyTypes = new HashMap<>();
        List<int[]> pathPoints = new ArrayList<>();
    }

    public static void main(String[] args) throws IOException {
        if (args.length < 3) {
            System.err.println("Uso: java TMXObjectParser <arquivo.tmx> <pasta_base_do_projeto> fase1_obj");
            return;
        }

        Path tmxPath = Paths.get(args[0]);
        Path incPath = Paths.get(args[1], args[2]);
        Path srcPath = Paths.get(args[1], args[2].replace(".h", ".c"));

        List<String> lines = Files.readAllLines(tmxPath);
        List<TMXObject> allObjects = parseObjects(lines);

        String headerContent = generateHeader(allObjects);
        String sourceContent = generateSource(allObjects, args[2]);

        Files.createDirectories(incPath.getParent());
        Files.createDirectories(srcPath.getParent());

        Files.write(incPath, headerContent.getBytes());
        Files.write(srcPath, sourceContent.getBytes());

        System.out.println("Arquivos gerados com sucesso.");
    }

    private static List<TMXObject> parseObjects(List<String> lines) {
        List<TMXObject> objects = new ArrayList<>();
        boolean inObjectGroup = false;
        TMXObject current = null;

        for (String line : lines) {
            line = line.trim();

            if (line.startsWith("<objectgroup") && line.contains("name=\"objetos\"")) {
                inObjectGroup = true;
            } else if (inObjectGroup && line.startsWith("</objectgroup")) {
                inObjectGroup = false;
            } else if (inObjectGroup && line.startsWith("<object")) {
                current = new TMXObject();
                current.name = extractStringAttribute(line, "name");
                String id = extractStringAttribute(line, "id");
                current.properties.put("__tmx_object_id", id);
                current.x = extractIntAttribute(line, "x");
                current.y = extractIntAttribute(line, "y");

                if (line.contains("polygon") || line.contains("polyline") || current.name.toLowerCase().contains("path")) {
                    current.properties.put("__object_x", String.valueOf(current.x));
                    current.properties.put("__object_y", String.valueOf(current.y));
                }

                objects.add(current);
            } else if (inObjectGroup && line.contains("<property")) {
                String key = extractStringAttribute(line, "name");
                String value = extractStringAttribute(line, "value");
                String type = extractStringAttribute(line, "type");

                current.properties.put(key, value);
                if (!type.isEmpty()) current.propertyTypes.put(key, type);
            } else if (inObjectGroup && (line.contains("<polyline") || line.contains("<polygon"))) {
                String pointsRaw = extractStringAttribute(line, "points");
                int baseX = Integer.parseInt(current.properties.getOrDefault("__object_x", "0"));
                int baseY = Integer.parseInt(current.properties.getOrDefault("__object_y", "0"));

                for (String point : pointsRaw.split(" ")) {
                    String[] xy = point.split(",");
                    int px = baseX + Math.round(Float.parseFloat(xy[0]));
                    int py = baseY + Math.round(Float.parseFloat(xy[1]));
                    current.pathPoints.add(new int[]{px, py});
                }
            }
        }

        return objects;
    }

    private static boolean isPathAgentCompatible(TMXObject obj) {
        return obj.properties.containsKey("path") &&
               obj.properties.containsKey("path_mode") &&
               obj.properties.containsKey("velocidade") &&
               obj.properties.containsKey("delayInicial");
    }

    private static String convertPathMode(String mode) {
        mode = mode.toLowerCase(Locale.ROOT);
        switch (mode) {
            case "loop": return "PATHMODE_LOOP";
            case "pingpong": return "PATHMODE_PINGPONG";
            case "oneshot": return "PATHMODE_ONESHOT";
            default: return "PATHMODE_LOOP";
        }
    }

    private static String generateHeader(List<TMXObject> objects) {
        StringBuilder sb = new StringBuilder();
        sb.append("#ifndef OBJETOS_H\n#define OBJETOS_H\n\n#include <genesis.h>\n#include \"components/path_agent.h\"\n#include \"components/path_def.h\"\n\n");

        Map<String, List<TMXObject>> grouped = new LinkedHashMap<>();
        Map<String, TMXObject> paths = new LinkedHashMap<>();

        for (TMXObject obj : objects) {
            if (!obj.pathPoints.isEmpty()) {
                String pathId = obj.properties.get("__tmx_object_id");
                sb.append("extern const Vect2D_u16 path_" + pathId + "[];\n");
                paths.putIfAbsent(pathId, obj);
            } else {
                grouped.computeIfAbsent(obj.name, k -> new ArrayList<>()).add(obj);
            }
        }

        for (Map.Entry<String, List<TMXObject>> entry : grouped.entrySet()) {
            String type = entry.getKey();
            TMXObject sample = entry.getValue().get(0);

            sb.append("typedef struct {\n");
            if (isPathAgentCompatible(sample)) {
                sb.append("    PathAgentDef agentDef;\n");
            }

            for (String key : sample.properties.keySet()) {
                if (key.startsWith("__") || key.equals("x") || key.equals("y") ||
                    key.equals("path") || key.equals("path_mode") ||
                    key.equals("velocidade") || key.equals("delayInicial") ||
                    key.equals("sprite") || key.equals("w") || key.equals("h"))
                    continue;

                sb.append("    " + inferCType(key, sample) + " " + key + ";\n");
            }

            sb.append("} " + type + "_t;\n\n");
            sb.append("extern const " + type + "_t " + type + "s[];\n");
        }

        sb.append("\n#endif // OBJETOS_H\n");
        return sb.toString();
    }

    private static String generateSource(List<TMXObject> objects, String filename) {
        StringBuilder sb = new StringBuilder();
        sb.append("#include \"" + filename + "\"\n\n");

        Map<String, List<TMXObject>> grouped = new LinkedHashMap<>();
        Map<String, TMXObject> paths = new LinkedHashMap<>();

        for (TMXObject obj : objects) {
            if (!obj.pathPoints.isEmpty()) {
                paths.putIfAbsent(obj.properties.get("__tmx_object_id"), obj);
            } else {
                grouped.computeIfAbsent(obj.name, k -> new ArrayList<>()).add(obj);
            }
        }

        for (Map.Entry<String, TMXObject> entry : paths.entrySet()) {
            sb.append("const Vect2D_u16 path_" + entry.getKey() + "[] = {\n");
            for (int[] pt : entry.getValue().pathPoints) {
                sb.append("    { " + pt[0] + ", " + pt[1] + " },\n");
            }
            sb.append("};\n\n");
        }

        for (Map.Entry<String, List<TMXObject>> entry : grouped.entrySet()) {
            String type = entry.getKey();
            List<TMXObject> objs = entry.getValue();
            TMXObject sample = objs.get(0);

            sb.append("const " + type + "_t " + type + "s[] = {\n");
            for (TMXObject obj : objs) {
                List<String> values = new ArrayList<>();

                if (isPathAgentCompatible(obj)) {
                    String pathId = obj.properties.get("path");
                    TMXObject pathObj = paths.get(pathId);
                    int pathLen = (pathObj != null) ? pathObj.pathPoints.size() : 0;

                    values.add(String.format(".agentDef = { " +
                            ".position = { %d, %d }, " +
                            ".path = path_%s, " +
                            ".path_len = %d, " +
                            ".delayInicial = %s, " +
                            ".path_mode = %s, " +
                            ".velocidade = FIX16(%s), " +
                            ".sprite = \"%s\", " +
                            ".w = %s, .h = %s }",
                        obj.x, obj.y,
                        pathId,
                        pathLen,
                        obj.properties.get("delayInicial"),
                        convertPathMode(obj.properties.get("path_mode")),
                        obj.properties.get("velocidade"),
                        obj.properties.getOrDefault("sprite", ""),
                        obj.properties.getOrDefault("w", "0"),
                        obj.properties.getOrDefault("h", "0")
                    ));
                }

                for (String key : sample.properties.keySet()) {
                    if (key.startsWith("__") || key.equals("x") || key.equals("y") ||
                        key.equals("path") || key.equals("path_mode") || key.equals("velocidade") ||
                        key.equals("delayInicial") || key.equals("sprite") || key.equals("w") || key.equals("h"))
                        continue;

                    values.add("." + key + " = " + formatValue(key, obj));
                }

                sb.append("    { " + String.join(", ", values) + " },\n");
            }
            sb.append("};\n\n");
        }

        return sb.toString();
    }

    private static int extractIntAttribute(String line, String attrName) {
        String search = attrName + "=\"";
        int start = line.indexOf(search);
        if (start == -1) return -1;
        start += search.length();
        int end = line.indexOf("\"", start);
        return (int) Math.round(Float.parseFloat(line.substring(start, end).replace(',', '.')));
    }

    private static String extractStringAttribute(String line, String attrName) {
        String search = attrName + "=\"";
        int start = line.indexOf(search);
        if (start == -1) return "";
        start += search.length();
        int end = line.indexOf("\"", start);
        return line.substring(start, end);
    }

    private static String inferCType(String key, TMXObject obj) {
        String type = obj.propertyTypes.getOrDefault(key, "");

        if (type.equals("bool")) return "bool";
        if (type.equals("float")) return "fix16";
        if (type.equals("int")) return "u8";

        String value = obj.properties.get(key);
        if (value.equals("true") || value.equals("false")) return "bool";
        try {
            Integer.parseInt(value);
            return "u8";
        } catch (NumberFormatException e) {
            return "char*";
        }
    }

    private static String formatValue(String key, TMXObject obj) {
        String value = obj.properties.get(key);
        String type = obj.propertyTypes.getOrDefault(key, "");

        if (type.equals("float")) {
            return "FIX16(" + value.replace(',', '.') + ")";
        }

        if (value.equals("true") || value.equals("false")) return value;

        try {
            Integer.parseInt(value);
            return value;
        } catch (NumberFormatException e) {
            return "\"" + value + "\"";
        }
    }
}
