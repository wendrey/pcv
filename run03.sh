#!/bin/bash
# EXACT
./color.e -e -i in03/fpsol2.i.1.col.fkm  -o out03/fpsol2.i.1.col.fkm.exact.out 
./color.e -e -i in03/fpsol2.i.2.col.fkm  -o out03/fpsol2.i.2.col.fkm.exact.out 
./color.e -e -i in03/fpsol2.i.3.col.fkm  -o out03/fpsol2.i.3.col.fkm.exact.out 
./color.e -e -i in03/inithx.i.1.col.fkm  -o out03/inithx.i.1.col.fkm.exact.out 
./color.e -e -i in03/inithx.i.2.col.fkm  -o out03/inithx.i.2.col.fkm.exact.out 
./color.e -e -i in03/inithx.i.3.col.fkm  -o out03/inithx.i.3.col.fkm.exact.out 
./color.e -e -i in03/queen15_15.col.fkm  -o out03/queen15_15.col.fkm.exact.out 
./color.e -e -i in03/queen16_16.col.fkm  -o out03/queen16_16.col.fkm.exact.out 
./color.e -e -i in03/school1.col.fkm     -o out03/school1.col.fkm.exact.out    
./color.e -e -i in03/school1_nsh.col.fkm -o out03/school1_nsh.col.fkm.exact.out
./color.e -e -i in03/zeroin.i.1.col.fkm  -o out03/zeroin.i.1.col.fkm.exact.out 
./color.e -e -i in03/zeroin.i.2.col.fkm  -o out03/zeroin.i.2.col.fkm.exact.out 
./color.e -e -i in03/zeroin.i.3.col.fkm  -o out03/zeroin.i.3.col.fkm.exact.out
# HEURISTIC
./color.e -h -i in03/fpsol2.i.1.col.fkm  -o out03/fpsol2.i.1.col.fkm.heuristic.out 
./color.e -h -i in03/fpsol2.i.2.col.fkm  -o out03/fpsol2.i.2.col.fkm.heuristic.out 
./color.e -h -i in03/fpsol2.i.3.col.fkm  -o out03/fpsol2.i.3.col.fkm.heuristic.out 
./color.e -h -i in03/inithx.i.1.col.fkm  -o out03/inithx.i.1.col.fkm.heuristic.out 
./color.e -h -i in03/inithx.i.2.col.fkm  -o out03/inithx.i.2.col.fkm.heuristic.out 
./color.e -h -i in03/inithx.i.3.col.fkm  -o out03/inithx.i.3.col.fkm.heuristic.out 
./color.e -h -i in03/queen15_15.col.fkm  -o out03/queen15_15.col.fkm.heuristic.out 
./color.e -h -i in03/queen16_16.col.fkm  -o out03/queen16_16.col.fkm.heuristic.out 
./color.e -h -i in03/school1.col.fkm     -o out03/school1.col.fkm.heuristic.out    
./color.e -h -i in03/school1_nsh.col.fkm -o out03/school1_nsh.col.fkm.heuristic.out
./color.e -h -i in03/zeroin.i.1.col.fkm  -o out03/zeroin.i.1.col.fkm.heuristic.out 
./color.e -h -i in03/zeroin.i.2.col.fkm  -o out03/zeroin.i.2.col.fkm.heuristic.out 
./color.e -h -i in03/zeroin.i.3.col.fkm  -o out03/zeroin.i.3.col.fkm.heuristic.out  
