# Performance

Below are some performance tests for iteration using the any_iterator.
There are no tests for comparing the std\::vector\::iterator, since the performance impact
of a simple pointer increment on a continious memory area is too huge and any_iterator was not desgined
for such a purporse.

Tests includes the performance impact given an iterator of std\::map or std\::list using the native vs. any iterator.
The third iterator, any_iterator_virtual, is an alternative implementation of any_iterator using an abstract class.

To summarize: any_iterator can do up to ~10% less iterations per timeunit than the native iterator.

Test machine:
4 X 3400 MHz CPUs (Intel i5-4670)
16GB RAM

Compiler: MSVC 2017, Release x64 (with default-config /O2)

![List Iteration](./benchmark_results/list_iteration.svg)

![Map Iteration](./benchmark_results/map_iteration.svg)


Raw Data for `std::list`:

| name/size                                      | iterations | real_time | cpu_time | time_unit | items_per_second | 
|---------------------------------------------------------------------------------------|------------|-----------|----------|-----------|------------------| 
| std::list<int>::iterator/256_mean              | 100000     | 5.53739   | 5.79688  | us        | 1.81123e+08      |   
| std::list<int>::iterator/256_median            | 100000     | 5.50829   | 5.78125  | us        | 1.77254e+08      |   
| std::list<int>::iterator/256_stddev            | 100000     | 0.0687006 | 0.927718 | us        | 2.98317e+07      |   
| std::list<int>::iterator/512_mean              | 66901      | 10.5749   | 10.6734  | us        | 1.94393e+08      |   
| std::list<int>::iterator/512_median            | 66901      | 10.5701   | 10.7435  | us        | 1.90627e+08      |   
| std::list<int>::iterator/512_stddev            | 66901      | 0.0276893 | 1.22442  | us        | 2.32154e+07      |   
| std::list<int>::iterator/4096_mean             | 6400       | 83.033    | 87.2803  | us        | 1.93025e+08      |   
| std::list<int>::iterator/4096_median           | 6400       | 82.9168   | 83.0078  | us        | 1.97379e+08      |   
| std::list<int>::iterator/4096_stddev           | 6400       | 0.338777  | 15.4991  | us        | 3.20862e+07      |   
| std::list<int>::iterator/32768_mean            | 896        | 686.065   | 677.49   | us        | 1.9551e+08       |   
| std::list<int>::iterator/32768_median          | 896        | 683.637   | 706.264  | us        | 1.85613e+08      |   
| std::list<int>::iterator/32768_stddev          | 896        | 6.0746    | 66.0478  | us        | 2.22956e+07      |   
| std::list<int>::iterator/262144_mean           | 75         | 7784.85   | 7750     | us        | 1.35393e+08      |   
| std::list<int>::iterator/262144_median         | 75         | 7707.48   | 7708.33  | us        | 1.36031e+08      |   
| std::list<int>::iterator/262144_stddev         | 75         | 194.408   | 209.427  | us        | 3.61043e+06      |   
| std::list<int>::iterator/2097152_mean          | 9          | 76091.9   | 75781.3  | us        | 1.10751e+08      |   
| std::list<int>::iterator/2097152_median        | 9          | 76038     | 76388.9  | us        | 1.09815e+08      |   
| std::list<int>::iterator/2097152_stddev        | 9          | 760.514   | 1715.43  | us        | 2.58219e+06      |   
| std::list<int>::iterator/8388608_mean          | 2          | 317940    | 317188   | us        | 1.05822e+08      |   
| std::list<int>::iterator/8388608_median        | 2          | 316387    | 320313   | us        | 1.04755e+08      |   
| std::list<int>::iterator/8388608_stddev        | 2          | 3796.72   | 5890.13  | us        | 1.96563e+06      |   
| tyti::any_iterator<int>256_mean               | 100000     | 6.04116   | 5.96094  | us        | 1.7503e+08       | 
| tyti::any_iterator<int>256_median             | 100000     | 6.04056   | 6.01563  | us        | 1.70482e+08      | 
| tyti::any_iterator<int>256_stddev             | 100000     | 0.0079298 | 0.827225 | us        | 2.49489e+07      | 
| tyti::any_iterator<int>512_mean               | 100000     | 11.8247   | 11.75    | us        | 1.755e+08        | 
| tyti::any_iterator<int>512_median             | 100000     | 11.7933   | 11.7969  | us        | 1.73613e+08      | 
| tyti::any_iterator<int>512_stddev             | 100000     | 0.0780091 | 0.969307 | us        | 1.54768e+07      | 
| tyti::any_iterator<int>4096_mean              | 7467       | 90.2938   | 92.6995  | us        | 1.79989e+08      | 
| tyti::any_iterator<int>4096_median            | 7467       | 90.0543   | 91.0255  | us        | 1.80017e+08      | 
| tyti::any_iterator<int>4096_stddev            | 7467       | 0.816185  | 12.4465  | us        | 2.5942e+07       | 
| tyti::any_iterator<int>32768_mean             | 1154       | 737.688   | 732.506  | us        | 1.79191e+08      | 
| tyti::any_iterator<int>32768_median           | 1154       | 737.515   | 724.383  | us        | 1.80959e+08      | 
| tyti::any_iterator<int>32768_stddev           | 1154       | 0.847756  | 28.4354  | us        | 6.9195e+06       | 
| tyti::any_iterator<int>262144_mean            | 90         | 8471.76   | 8550.35  | us        | 1.23199e+08      | 
| tyti::any_iterator<int>262144_median          | 90         | 8471.93   | 8333.33  | us        | 1.25829e+08      | 
| tyti::any_iterator<int>262144_stddev          | 90         | 57.7204   | 616.709  | us        | 8.25443e+06      | 
| tyti::any_iterator<int>2097152_mean           | 9          | 78455.4   | 78385.4  | us        | 1.07034e+08      | 
| tyti::any_iterator<int>2097152_median         | 9          | 78226.3   | 78125    | us        | 1.07374e+08      | 
| tyti::any_iterator<int>2097152_stddev         | 9          | 608.493   | 1019.35  | us        | 1.36534e+06      | 
| tyti::any_iterator<int>8388608_mean           | 2          | 331906    | 330469   | us        | 1.01559e+08      | 
| tyti::any_iterator<int>8388608_median         | 2          | 331573    | 328125   | us        | 1.02261e+08      | 
| tyti::any_iterator<int>8388608_stddev         | 2          | 2950.88   | 5132.4   | us        | 1.56007e+06      | 
| tyti::any_iterator_virtual<int>/256_mean       | 74667      | 7.90783   | 7.79503  | us        | 1.33711e+08      | 
| tyti::any_iterator_virtual<int>/256_median     | 74667      | 7.87897   | 7.74271  | us        | 1.32253e+08      | 
| tyti::any_iterator_virtual<int>/256_stddev     | 74667      | 0.0826192 | 1.03162  | us        | 1.90468e+07      | 
| tyti::any_iterator_virtual<int>/512_mean       | 44800      | 15.4446   | 15.5552  | us        | 1.3324e+08       | 
| tyti::any_iterator_virtual<int>/512_median     | 44800      | 15.4483   | 15.6948  | us        | 1.30489e+08      | 
| tyti::any_iterator_virtual<int>/512_stddev     | 44800      | 0.0143917 | 1.56958  | us        | 1.68216e+07      | 
| tyti::any_iterator_virtual<int>/4096_mean      | 5600       | 117.693   | 111.607  | us        | 1.49059e+08      | 
| tyti::any_iterator_virtual<int>/4096_median    | 5600       | 117.47    | 110.212  | us        | 1.48874e+08      | 
| tyti::any_iterator_virtual<int>/4096_stddev    | 5600       | 1.05071   | 14.2272  | us        | 1.8727e+07       | 
| tyti::any_iterator_virtual<int>/32768_mean     | 1000       | 941.824   | 953.125  | us        | 1.53194e+08      | 
| tyti::any_iterator_virtual<int>/32768_median   | 1000       | 941.976   | 1000     | us        | 1.31104e+08      | 
| tyti::any_iterator_virtual<int>/32768_stddev   | 1000       | 3.08549   | 277.339  | us        | 6.11236e+07      | 
| tyti::any_iterator_virtual<int>/262144_mean    | 64         | 10305.7   | 10376    | us        | 1.01139e+08      | 
| tyti::any_iterator_virtual<int>/262144_median  | 64         | 10305.3   | 10376    | us        | 1.01072e+08      | 
| tyti::any_iterator_virtual<int>/262144_stddev  | 64         | 131.449   | 301.621  | us        | 2.94572e+06      | 
| tyti::any_iterator_virtual<int>/2097152_mean   | 7          | 90958.7   | 89732.1  | us        | 9.36174e+07      | 
| tyti::any_iterator_virtual<int>/2097152_median | 7          | 90733.9   | 89285.7  | us        | 9.39524e+07      | 
| tyti::any_iterator_virtual<int>/2097152_stddev | 7          | 766.723   | 3442.82  | us        | 3.6373e+06       | 
| tyti::any_iterator_virtual<int>/8388608_mean   | 2          | 377142    | 377344   | us        | 8.89307e+07      | 
| tyti::any_iterator_virtual<int>/8388608_median | 2          | 376218    | 375000   | us        | 8.94785e+07      | 
| tyti::any_iterator_virtual<int>/8388608_stddev | 2          | 3043.57   | 3673.14  | us        | 858559           | 


Raw Data for `std::map`:

| name/size                                                            | iterations | real_time  | cpu_time | time_unit | items_per_second | 
|------------------------------------------------------------------------------------------------|------------|------------|----------|-----------|------------------| 
| std::map<int, int>::iterator>/256_mean                               | 64000      | 6.95075    | 7.15332  | us        | 1.45587e+08      |
| std::map<int, int>::iterator>/256_median                             | 64000      | 6.9528     | 6.83594  | us        | 1.49797e+08      |
| std::map<int, int>::iterator>/256_stddev                             | 64000      | 0.00816309 | 0.976884 | us        | 1.88976e+07      |
| std::map<int, int>::iterator>/512_mean                               | 44800      | 13.8113    | 14.2822  | us        | 1.46193e+08      |
| std::map<int, int>::iterator>/512_median                             | 44800      | 13.7986    | 13.7765  | us        | 1.48683e+08      |
| std::map<int, int>::iterator>/512_stddev                             | 44800      | 0.0563736  | 2.08489  | us        | 2.05136e+07      |
| std::map<int, int>::iterator>/4096_mean                              | 4978       | 108.335    | 115.665  | us        | 1.44778e+08      |
| std::map<int, int>::iterator>/4096_median                            | 4978       | 108        | 112.997  | us        | 1.44995e+08      |
| std::map<int, int>::iterator>/4096_stddev                            | 4978       | 0.841261   | 17.3204  | us        | 2.22359e+07      |
| std::map<int, int>::iterator>/32768_mean                             | 797        | 935.983    | 923.385  | us        | 1.46766e+08      |
| std::map<int, int>::iterator>/32768_median                           | 797        | 934.232    | 911.622  | us        | 1.43929e+08      |
| std::map<int, int>::iterator>/32768_stddev                           | 797        | 5.77633    | 169.114  | us        | 2.85722e+07      |
| std::map<int, int>::iterator>/262144_mean                            | 45         | 12918.7    | 12725.7  | us        | 8.3135e+07       |
| std::map<int, int>::iterator>/262144_median                          | 45         | 12859.2    | 12673.6  | us        | 8.27525e+07      |
| std::map<int, int>::iterator>/262144_stddev                          | 45         | 219.682    | 1225.16  | us        | 8.11041e+06      |
| std::map<int, int>::iterator>/2097152_mean                           | 6          | 113382     | 113281   | us        | 7.41105e+07      |
| std::map<int, int>::iterator>/2097152_median                         | 6          | 113284     | 113281   | us        | 7.40609e+07      |
| std::map<int, int>::iterator>/2097152_stddev                         | 6          | 530.18     | 3326.39  | us        | 2.12949e+06      |
| std::map<int, int>::iterator>/8388608_mean                           | 1          | 496598     | 496875   | us        | 6.75615e+07      |
| std::map<int, int>::iterator>/8388608_median                         | 1          | 492880     | 500000   | us        | 6.71089e+07      |
| std::map<int, int>::iterator>/8388608_stddev                         | 1          | 9276.98    | 10872.7  | us        | 1.47101e+06      |
| tyti::any_iterator<std::pair<const int, int>>>/256_mean              | 74667      | 7.51286    | 7.5753   | us        | 1.38086e+08      |
| tyti::any_iterator<std::pair<const int, int>>>/256_median            | 74667      | 7.51114    | 7.53345  | us        | 1.35927e+08      |
| tyti::any_iterator<std::pair<const int, int>>>/256_stddev            | 74667      | 0.0142167  | 1.16136  | us        | 2.04124e+07      |
| tyti::any_iterator<std::pair<const int, int>>>/512_mean              | 56000      | 14.7222    | 15.4157  | us        | 1.36296e+08      |
| tyti::any_iterator<std::pair<const int, int>>>/512_median            | 56000      | 14.7191    | 14.5089  | us        | 1.41154e+08      |
| tyti::any_iterator<std::pair<const int, int>>>/512_stddev            | 56000      | 0.0179606  | 2.45401  | us        | 2.33817e+07      |
| tyti::any_iterator<std::pair<const int, int>>>/4096_mean             | 6400       | 115.312    | 108.154  | us        | 1.54421e+08      |
| tyti::any_iterator<std::pair<const int, int>>>/4096_median           | 6400       | 115.216    | 108.643  | us        | 1.50825e+08      |
| tyti::any_iterator<std::pair<const int, int>>>/4096_stddev           | 6400       | 0.405724   | 14.672   | us        | 2.32509e+07      |
| tyti::any_iterator<std::pair<const int, int>>>/32768_mean            | 747        | 994.036    | 996.695  | us        | 1.40968e+08      |
| tyti::any_iterator<std::pair<const int, int>>>/32768_median          | 747        | 990.609    | 1066.77  | us        | 1.22916e+08      |
| tyti::any_iterator<std::pair<const int, int>>>/32768_stddev          | 747        | 19.7649    | 254.491  | us        | 4.05995e+07      |
| tyti::any_iterator<std::pair<const int, int>>>/262144_mean           | 50         | 13368.4    | 13296.9  | us        | 7.89341e+07      |
| tyti::any_iterator<std::pair<const int, int>>>/262144_median         | 50         | 13363.5    | 13281.3  | us        | 7.89625e+07      |
| tyti::any_iterator<std::pair<const int, int>>>/262144_stddev         | 50         | 49.8128    | 423.835  | us        | 2.48733e+06      |
| tyti::any_iterator<std::pair<const int, int>>>/2097152_mean          | 6          | 117846     | 116927   | us        | 7.18384e+07      |
| tyti::any_iterator<std::pair<const int, int>>>/2097152_median        | 6          | 117559     | 117188   | us        | 7.15828e+07      |
| tyti::any_iterator<std::pair<const int, int>>>/2097152_stddev        | 6          | 809.059    | 4382.11  | us        | 2.70335e+06      |
| tyti::any_iterator<std::pair<const int, int>>>/8388608_mean          | 1          | 509935     | 509375   | us        | 6.59066e+07      |
| tyti::any_iterator<std::pair<const int, int>>>/8388608_median        | 1          | 506596     | 500000   | us        | 6.71089e+07      |
| tyti::any_iterator<std::pair<const int, int>>>/8388608_stddev        | 1          | 8839.94    | 11780.3  | us        | 1.49836e+06      |
| tyti::any_iterator_virtual<std::pair<const int,int>>>/256_mean       | 112000     | 9.41279    | 9.60519  | us        | 1.07617e+08      |
| tyti::any_iterator_virtual<std::pair<const int,int>>>/256_median     | 112000     | 9.40681    | 9.27734  | us        | 1.10383e+08      |
| tyti::any_iterator_virtual<std::pair<const int,int>>>/256_stddev     | 112000     | 0.0274856  | 0.938887 | us        | 1.09339e+07      |
| tyti::any_iterator_virtual<std::pair<const int,int>>>/512_mean       | 56000      | 18.4665    | 18.3873  | us        | 1.12549e+08      |
| tyti::any_iterator_virtual<std::pair<const int,int>>>/512_median     | 56000      | 18.4528    | 18.5547  | us        | 1.10383e+08      |
| tyti::any_iterator_virtual<std::pair<const int,int>>>/512_stddev     | 56000      | 0.0483194  | 1.91798  | us        | 1.18615e+07      |
| tyti::any_iterator_virtual<std::pair<const int,int>>>/4096_mean      | 6175       | 144.952    | 141.447  | us        | 1.17562e+08      |
| tyti::any_iterator_virtual<std::pair<const int,int>>>/4096_median    | 6175       | 144.785    | 141.7    | us        | 1.15624e+08      |
| tyti::any_iterator_virtual<std::pair<const int,int>>>/4096_stddev    | 6175       | 0.735478   | 17.5097  | us        | 1.48448e+07      |
| tyti::any_iterator_virtual<std::pair<const int,int>>>/32768_mean     | 498        | 1232.21    | 1214.23  | us        | 1.0855e+08       |
| tyti::any_iterator_virtual<std::pair<const int,int>>>/32768_median   | 498        | 1228.92    | 1192.27  | us        | 1.09935e+08      |
| tyti::any_iterator_virtual<std::pair<const int,int>>>/32768_stddev   | 498        | 12.0076    | 92.796   | us        | 8.35497e+06      |
| tyti::any_iterator_virtual<std::pair<const int,int>>>/262144_mean    | 50         | 15363.3    | 15250    | us        | 6.88048e+07      |
| tyti::any_iterator_virtual<std::pair<const int,int>>>/262144_median  | 50         | 15366.4    | 15312.5  | us        | 6.84784e+07      |
| tyti::any_iterator_virtual<std::pair<const int,int>>>/262144_stddev  | 50         | 46.1666    | 400.452  | us        | 1.83459e+06      |
| tyti::any_iterator_virtual<std::pair<const int,int>>>/2097152_mean   | 5          | 133362     | 133125   | us        | 6.30354e+07      |
| tyti::any_iterator_virtual<std::pair<const int,int>>>/2097152_median | 5          | 133070     | 134375   | us        | 6.24269e+07      |
| tyti::any_iterator_virtual<std::pair<const int,int>>>/2097152_stddev | 5          | 893.527    | 2564.95  | us        | 1.22306e+06      |
| tyti::any_iterator_virtual<std::pair<const int,int>>>/8388608_mean   | 1          | 571101     | 573438   | us        | 5.85447e+07      |
| tyti::any_iterator_virtual<std::pair<const int,int>>>/8388608_median | 1          | 566940     | 570313   | us        | 5.88462e+07      |
| tyti::any_iterator_virtual<std::pair<const int,int>>>/8388608_stddev | 1          | 10272.3    | 13507.9  | us        | 1.34607e+06      |