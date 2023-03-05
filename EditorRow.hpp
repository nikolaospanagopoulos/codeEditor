#pragma once
#include <string>
class EditorRow {
public:
  int size;
  std::string *rowText;
  std::string *render;
  EditorRow();
  ~EditorRow();
};
