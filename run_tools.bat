ECHO Exporting TILED collision map
java -cp ../tools TMXToCollisionHeader ../Tiled/Teste04/Teste04.tmx ../SMDX/src/tiled fase1_col.h --rle

ECHO Exporting TILED objects
java -cp ../tools TMXObjectParserSplitOutput ../Tiled/Teste04/Teste04.tmx ../SMDX/src/tiled/ fase1_obj.h

ECHO Exporting TILED slopes
java -cp ../tools TmxSlopeExporter ../Tiled/Teste04/Teste04.tmx ../SMDX/src/tiled/fase1_slopes.h
