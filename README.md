# fighter_sim
fighter jets simulation

Run the following at the root:
```
./run_demo.sh <data_dir>
./run_gamer.sh <data_dir>
./run_checker.sh <data_dir>
```

Example:
./run_gamer.sh data

To compile (included in run_gamer.sh):
```
cd src
make clean
make
```

Each gamer run will also output a ```debug.txt```, which has necessary debuging information. You need to compile with ```DEBUG``` macro enabled in the ```gamer.cpp```. 