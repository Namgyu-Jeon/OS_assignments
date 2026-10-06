// 부모와 자식을 들여쓰기와 +, -, | 선으로 구분해서 보여 주는 함수를 선언한다.
#pragma once

#include "ProcessTree.h"

#include <iosfwd>

// 부모를 먼저 보여 주고 그 아래의 자식들을 끝까지 보여 준 뒤, 같은 부모의 다음 자식으로 넘어간다.
// 출력할 내용은 output에 쓰므로, main에서는 화면 출력을 연결해 사용한다.
void PrintProcessTree(const ProcessTree& tree, std::wostream& output);
