#include <regex>
#include <string>
using namespace std;

regex placeRegex("^\\-place\\=([a-zA-Z][a-zA-Z0-9_]*)@(-?[0-9]+|0x[0-9A-F]+)$");
regex rg_STLine("^TS\\-([a-zA-Z][a-zA-Z0-9_]*):(-?[0-9]+):(-?[0-9]+):(-?[0-9]+):([a-zA-Z][a-zA-Z0-9_]*)$");
regex rg_SNLine("SN\\-([a-zA-Z][a-zA-Z0-9_]*)$");
regex rg_RTLine("^TR\\-(-?[0-9]+):(-?[0-9]+):([a-zA-Z][a-zA-Z0-9_]*):(-?[0-9]+):([a-zA-Z][a-zA-Z0-9_]*)$");
regex rg_SDLine("^SD\\-([0-9A-Fa-f]+|xx):([0-9A-Fa-f]+|xx):([0-9A-Fa-f]+|xx):([0-9A-Fa-f]+|xx):([0-9A-Fa-f]+|xx):([0-9A-Fa-f]+|xx):([0-9A-Fa-f]+|xx):([0-9A-Fa-f]+|xx)$");