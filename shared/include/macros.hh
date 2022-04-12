#pragma once

#define export_func __attribute__((visibility("default"))) extern "C"
#define import_func extern "C"
