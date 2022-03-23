# mapbench

This is a simple C++ application for testing the speed of counting incoming connections and token tracking.

### Results

| Connections | Tokens | MAP |
| ------ | ------ |  ------ |  
| 8.70492s | 20.9568s | STL map based on Red-Black Tree. |
| 2.39977s | 6.05628s | STL unordered_map based on Hash Table. |
| 2.40172s | 5.83537s | Boost unordered_map based on Hash container. |
| 7.96565s | 28.7991s | My map implementaion based on AVL(Adelson-Velsky and Landis) Tree. |

## Conclusion
For this project it is preferable to use unordered_map.