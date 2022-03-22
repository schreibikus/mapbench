# mapbench
This is a simple C++ application for testing the speed of counting incoming connections.
Everything is simple.
Userid is present by unique number and we create a map for counting count requests from users.

##Results
8.37961s - STL map based on Red-Black Tree.
2.34829s - STL unordered_map based on Hash Table.
2.19521s - Boost unordered_map based on Hash container.
7.00824s - My map implementaion based on AVL(Adelson-Velsky and Landis) Tree.

##Conclusion
In this case, when Userid values are concentrated in one small range(0-1000000), it is preferable to use an unordered_map.

