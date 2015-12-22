#!/bin/bash

./cs311sim -nobp -f -n 10000 $1 > out_nobp_f
./cs311test -nobp -f -n 10000 $1 > sol_nobp_f

./cs311sim -nobp -n 10000 $1 > out_nobp
./cs311test -nobp -n 10000 $1 > sol_nobp

./cs311sim -f -n 10000 $1 > out_f
./cs311test -f -n 10000 $1 > sol_f

./cs311sim -n 10000 $1 > out
./cs311test -f -n 10000 $1 > sol

diff out_nobp_f sol_nobp_f > report/nobp_f
diff out_nobp sol_nobp > report/nobp
diff out_f sol_f > report/f
diff out sol > report/none

mv out trace/
mv out_* trace/
mv sol trace/
mv sol_* trace/

ls -l report/
