import org.w3c.dom.*;
import javax.xml.parsers.*;
import java.io.*;
import java.util.*;
import java.nio.file.*;

public class TmxSlopeExporter {

    static class Slope {
        int x1, y1, x2, y2;
        int type;

        Slope(int x1, int y1, int x2, int y2, int type) {
            this.x1 = x1; this.y1 = y1;
            this.x2 = x2; this.y2 = y2;
            this.type = type;
        }

        String toC() {
            return String.format("    { %d, %d, %d, %d, %d }", x1, y1, x2, y2, type);
        }
    }

    public static void main(String[] args) throws Exception {
        if (args.length != 2) {
            System.err.println("Uso: java TmxSlopeExporter mapa.tmx saida.h");
            System.exit(1);
        }

        String inputFile = args[0];
        String outputFile = args[1];

        List<Slope> slopes = new ArrayList<>();

        DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
        DocumentBuilder dBuilder = dbFactory.newDocumentBuilder();
        Document doc = dBuilder.parse(new File(inputFile));
        doc.getDocumentElement().normalize();

        NodeList objectGroups = doc.getElementsByTagName("objectgroup");
        for (int i = 0; i < objectGroups.getLength(); i++) {
            Element group = (Element) objectGroups.item(i);
            if (!group.getAttribute("name").equals("Slopes")) continue;

            NodeList objects = group.getElementsByTagName("object");
            for (int j = 0; j < objects.getLength(); j++) {
                Element obj = (Element) objects.item(j);

                int objX = (int) Float.parseFloat(obj.getAttribute("x"));
                int objY = (int) Float.parseFloat(obj.getAttribute("y"));

                // Get slope type (optional)
                int type = 0;
                NodeList props = obj.getElementsByTagName("property");
                for (int p = 0; p < props.getLength(); p++) {
                    Element prop = (Element) props.item(p);
                    if (prop.getAttribute("name").equals("slopeType")) {
                        String val = prop.getAttribute("value");
                        if (val.contains("down")) type = 1;
                    }
                }

                // Get polyline
                NodeList polylines = obj.getElementsByTagName("polyline");
                if (polylines.getLength() == 0) continue;

                Element poly = (Element) polylines.item(0);
                String[] points = poly.getAttribute("points").split(" ");

                if (points.length < 2) continue;
                String[] p1 = points[0].split(",");
                String[] p2 = points[1].split(",");

                int x1 = objX + (int) Float.parseFloat(p1[0]);
                int y1 = objY + (int) Float.parseFloat(p1[1]);
                int x2 = objX + (int) Float.parseFloat(p2[0]);
                int y2 = objY + (int) Float.parseFloat(p2[1]);

                slopes.add(new Slope(x1, y1, x2, y2, type));
            }
        }

        // Escreve o header
        StringBuilder out = new StringBuilder();
        out.append("#ifndef _SLOPES_H_\n#define _SLOPES_H_\n\n");
        out.append("#include <genesis.h>\n");
        out.append("#include \"physics/physic_def.h\"\n\n");
/*
        out.append("typedef struct {\n");
        out.append("    s16 x1, y1;\n");
        out.append("    s16 x2, y2;\n");
        out.append("    u8 type;\n");
        out.append("} Slope;\n\n");
*/
        out.append("#define SLOPE_COUNT " + slopes.size() + "\n");
        out.append("const Slope slopes[SLOPE_COUNT] = {\n");
        for (int i = 0; i < slopes.size(); i++) {
            out.append(slopes.get(i).toC());
            out.append((i < slopes.size() - 1) ? ",\n" : "\n");
        }
        out.append("};\n\n");
        out.append("#endif\n");

        Files.write(Paths.get(outputFile), out.toString().getBytes());
        System.out.println("Exportado com sucesso para: " + outputFile);
    }
}
