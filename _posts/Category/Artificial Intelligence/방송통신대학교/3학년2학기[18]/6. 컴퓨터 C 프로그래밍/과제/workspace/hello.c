#include <stdio.h>
#include <wchar.h>
#include <locale.h>
/*
컴퓨터가 1부터 100 사이의 난수를 생성하고, 사용자가 이 숫자를 맞추는 게임을 만드세요.
사용자가 숫자를 입력할 때마다 크거나 작음을 알려주고, 사용자가 숫자를 맞추면 게임은 일단 종료됩니다.
어떤 숫자를 어떤 과정을 통해 몇 번 만에 맞췄는지 결과를 출력하고, 게임을 계속할 것인지 사용자에게 물어봅니다.
프로그램 작성에 필요하다면 스스로 가정을 만들고 해결하세요.

-과제물은 “C1_학번_이름.zip”으로 압축하여 제출

-압축 파일에는 모든 소스 프로그램 파일과 한글(또는 워드) 파일을 포함시키세요.

-한글(또는 워드) 파일에는 프로그램에 관한 1쪽 이내의 설명과 모든 소스코드(텍스트)를 넣고
    “비쥬얼스튜디오 편집기 화면” 및 “실행 화면”을 캡처한 이미지를 포함시키세요.

-소스 프로그램과 한글(또는 워드) 파일의 이름은 임의로 정하고, 비쥬얼스튜디오 외 다른 도구를 사용해도 무방합니다.
    비쥬얼스튜디오가 아닌 다른 도구를 사용한 경우, 사용 도구에 관한 간단한 설명도 추가하세요.
*/
int main(int argc, char const *argv[])
{
    system("chcp 65001");
    // 1~100 사이 난수 생성
    int random = rand() % 100;
    printf("%d", random);
    printf("Hello World~!ssss");
    return 0;
}