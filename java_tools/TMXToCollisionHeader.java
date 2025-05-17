import java.io.*;
import java.nio.file.*;
import java.util.*;

public class TMXToCollisionHeader {

    public static void main(String[] args) throws IOException {
        if (args.length < 3) {
            System.err.println("Uso: java TMXToCollisionHeader <arquivo.tmx> <pasta_destino> <nome_header> [--rle]");
            return;
        }

        boolean useRLE = args.length >= 4 && args[3].equalsIgnoreCase("--rle");

        Path tmxPath = Paths.get(args[0]);
        Path outputDir = Paths.get(args[1]);
        String headerName = args[2];

        List<String> lines = Files.readAllLines(tmxPath);
        int width = 0, height = 0;
        boolean inCollisionLayer = false;
        boolean inData = false;
        StringBuilder csvData = new StringBuilder();

        for (String line : lines) {
            line = line.trim();
            if (line.startsWith("<layer") && line.contains("name=\"collision\"")) {
                inCollisionLayer = true;
                width = extractIntAttribute(line, "width");
                height = extractIntAttribute(line, "height");
            } else if (inCollisionLayer && line.startsWith("<data") && line.contains("encoding=\"csv\"")) {
                inData = true;
            } else if (inData && line.startsWith("</data>")) {
                inData = false;
                inCollisionLayer = false;
            } else if (inData) {
                csvData.append(line);
            }
        }

        String[] rawValues = csvData.toString().split(",");
        if (rawValues.length != width * height)
            throw new RuntimeException("Número de valores incompatível com as dimensões do mapa.");

        List<Integer> original = new ArrayList<>();
        for (String val : rawValues)
            original.add(Integer.parseInt(val.trim()));

        Path outputPath = outputDir.resolve(headerName);
        try (FileWriter writer = new FileWriter(outputPath.toFile())) {
            writer.write("#pragma once\n");
            writer.write("#include <genesis.h>\n");
            writer.write("#include \"tiled_map.h\"\n\n");

            writer.write("const CollisionArray " + headerName.replace(".h", "") + " = {\n");

            if (useRLE) {
                List<Integer> rleData = compressRLE(original);
                writer.write("    .data = (const u8[]){\n        ");
                for (int i = 0; i < rleData.size(); i++) {
                    writer.write(rleData.get(i).toString());
                    if (i < rleData.size() - 1) writer.write(", ");
                }
                writer.write("\n    },\n");
            } else {
                writer.write("    .data = (const u8[]){\n");
                for (int y = 0; y < height; y++) {
                    writer.write("        ");
                    for (int x = 0; x < width; x++) {
                        int index = y * width + x;
                        writer.write(original.get(index).toString());
                        if (index < original.size() - 1) writer.write(", ");
                    }
                    writer.write("\n");
                }
                writer.write("    },\n");
            }

            writer.write("    .width = " + width + ",\n");
            writer.write("    .height = " + height + ",\n");
            writer.write("    .compressed = " + (useRLE ? "TRUE":"FALSE") + "\n");
            writer.write("};\n");
        }

        System.out.println("Header gerado com sucesso: " + headerName);
    }

    private static int extractIntAttribute(String line, String attrName) {
        String search = attrName + "=\"";
        int start = line.indexOf(search) + search.length();
        int end = line.indexOf("\"", start);
        return Integer.parseInt(line.substring(start, end));
    }

    private static List<Integer> compressRLE(List<Integer> data) {
    List<Integer> result = new ArrayList<>();
    int i = 0;

    while (i < data.size()) {
        int value = data.get(i);
        int runLength = 1;

        // Conta quantos valores iguais consecutivos existem
        while (i + runLength < data.size() && data.get(i + runLength) == value) {
            runLength++;
        }

        if (runLength > 1) {
            // Divide em blocos de até 128
            int chunk = 0;
            while (runLength > 0) {
                chunk = Math.min(runLength, 128); // 128 = máximo suportado
                result.add((0x80 | (chunk - 1)) & 0xFF); // prefixo RLE
                result.add(value & 0xFF);                // valor
                runLength -= chunk;
            }
            i += runLength == 0 ? chunk : 0;
        } else {
            result.add(value & 0xFF); // valor literal
            i++;
        }
    }

    return result;
}

}
