#pragma once
#include <string>
class EditorRow {
public:
  int size;
  std::string *rowText;

  EditorRow();
  ~EditorRow();
};
