rm -rf pp && mkdir pp
make triangle_conveyor
oshrun -n 20 ./triangle_conveyor -e 0 -n 10000
oshrun -n 20 ./triangle_conveyor -e 0 -n 25000
oshrun -n 20 ./triangle_conveyor -e 0 -n 75000
oshrun -n 20 ./triangle_conveyor -e 0 -n 100000



