#include "EditorRow.hpp"
EditorRow::EditorRow() : size(0) { rowText = new std::string{}; }
EditorRow::~EditorRow() { delete rowText; }
