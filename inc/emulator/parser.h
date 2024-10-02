#include <regex>
using namespace std;


regex rg_8MemoryLocation("^([0-9A-Fa-f]+):\\s*([0-9A-Fa-f]+|xx):([0-9A-Fa-f]+|xx):([0-9A-Fa-f]+|xx):([0-9A-Fa-f]+|xx):([0-9A-Fa-f]+|xx):([0-9A-Fa-f]+|xx):([0-9A-Fa-f]+|xx):([0-9A-Fa-f]+|xx)$");