			OS/2용 한글 편집기
		       HanEdit/2  Ver 0.051

0.개요

	OS/2 PM상에서 돌아가는 한글 에디터입니다.상용조합형과 KS-5601
완성형,두벌식과 세벌식 키보드(390,최종)을 지원합니다.

1.기능

	상용조합형과 KSC-5601 완성형 문서를 편집할 수 있습니다. 현재 
한글코드의 자동인식이 지원되지만, 완벽하지는 않습니다. 디폴트는 완성형
편집이며, 실행시에 -j 옵션을 주면 조합형 편집상태로 시작합니다.

	특수문자와 한자의 읽기와 입력및 저장이 가능합니다.

	OS/2형 아스키화일(CR/LF)와 유닉스형 아스키화일(LF)가 모두 지원
됩니다(File->Options). 단, 읽어온 문서를 저장할 경우에는 CR-LF/LF 종류
가 보존되지 않으며,현재 편집상태에 따라 저장됩니다. 즉 LF 형식의 문서
를 읽어왔더라도 편집모드가 CR/LF 인 상태로 저장을 하면 CR/LF 형식으로 
바뀌어 기록됩니다. 이것은 완성형/조합형 형식 구분에도 동일하게 적용됩
니다.

	Drag & Drop 이 가능합니다.
  
	알아서 줄바꾸기(Word Wrap) 기능이 지원됩니다.

2.옵션

	-w	:  KSC-5601 완성형 편집모드(기본값)
	-j	:  상용조합형 편집모드
	-kbd2	:  두벌식 (기본값)
	-kbd390	:  세벌식 390
	-kbd3f	:  세벌식 최종
	-crlf	:  OS/2,DOS,Windows형 텍스트 (CR-LF) 편집모드 (기본값)
	-lf	:  Unix형 텍스트 (LF) 편집모드
	-readonly,-no-readonly
		:  읽기 전용 / 쓰기 가능(기본값)
	-no-toolbar,-no-statbar
		:  툴바나 상태표시줄을 나타나지 않게 한다.
	-autoindent,-no-autoindent
		:  AutoIndent 기능을 사용/사용하지 않음
	-fg <color>,-bg <color>
		:  색을 지정한다.(기본값: -fg white -bg darkblue)
		   (black,blue,red,pink,green,yellow,cyan,lightgray,
		    darkgray,darkblue,darkred,darkpink,darkgreen,
		    darkyellow,darkcyan,white)
	-hfont <fontpath>,-efont <fontpath>,
		-specfont <fontpath>,-hanjafont <fontpath>
		:  외부 비트맵 폰트를 사용한다.
		   한글,영문,특수문자,한자 폰트화일들은 헤더 없이
		   각각 11520,4096,36096,156416 바이트 크기를 갖는다.
	-x <xpos>,-y <ypos>,-col <col>,-line <line>
		:  창의 초기 위치와 크기를 지정한다.
		   (기본값: -x 1 -y 1 -col 70 -line 20)
		   위치는 pixel단위,크기는 column/line단위이다.
	-cp <codepage>
		:  코드페이지를 변환한다.(테스트 필요)
                
	-tabsize <tabsize>
        	:  탭 크기를 지정한다.
                   기본 크기는 8 이다.
                   
	-wordwrap <wordwrapsize>
        	:  알아서 줄바꾸기를 할 때, 한 줄 길이를 정한다.
                   기본은 알아서 바꾸지 않는다. 
                   0 을 지정할 경우, 화면 크기를 기준으로 한다.

3.환경변수

	HANEDIT2OPT
		:  hanedit가 실행될때 이 변수값을 기본명령행 옵션으로
		   사용한다.실제 명령행 옵션에 의해 변경될 수 있다.

		예:
		   C:>SET HANEDIT2OPT=-fg red -bg black
		   C:>hanedit -fg green
		    --> 글자색은 green,배경색은 black으로 시작한다.


4.키입력

	한/영 전환              Shift-Space
	두벌식/세벌식 전환      Ctrl-Space
	불러오기                Ctrl-O
	저장                    Ctrl-S
	찾기/바꾸기             Ctrl-F
	종료                    Alt-X
	한글코드바꾸기          F2
	특수문자 입력           F4
	한자입력및 한글변환     F9
	클립보드로 복사         Ctrl-Insert
	클립보드로 잘라내기     Shift-Delete
	클립보드로부터 붙이기   Shift-Insert


5.주의사항

	.아직 숨어있는 버그가 있을 수 있습니다. 중요한 문서를 편집하는
경우에는 사용하지 마세요.(-readonly 옵션을 사용해서 한글뷰어로만 사용
하면 안전하겠죠)

	.완성형 편집모드에서, 조합형에만 존재하는 글자도 일단 입력은 
가능하나, 저장시엔 공백으로 처리됩니다.

	.완성형<->조합형 편집상태 전환시 문서의 한글코드는 자동으로 변
환되지 않습니다. 가령 완성형 편집상태에서 조합형 문서를 불러오면 한글
코드가 다르므로 글자가 깨져 보이게 되는데, 이때 다시 조합형 편집상태로
전환해도 문서는 여전히 깨져 보일 것입니다. 이 때는 조합형 편집 상태에
서 다시 문서(조합형)을 읽어와야 합니다. File->Reload as KS/johab 메뉴
를 이용하셔도 됩니다.

	.버젼 0.04 부터는 읽기전용 모드를 위해 -v 옵션 대신 -readonly 
옵션을 사용합니다. -v 옵션은 사용할 수 없습니다.

6.기타

	 본 프로그램은 공개 프로그램입니다.자유롭게 사용하실 수 있으며
, 배포하실때는 본 문서를 반드시 포함시켜 주십시오.그리고 제작자는 본 
프로그램의 사용으로 발생한 어떠한 피해에도 책임지지 않습니다.



                              버그 리포트 - Hitel,Nownuri ID : kwisatz
                                                kwisatz@mail.hitel.net
					  	          제작: 문유성
                                                
                                              천리안, 하이텔 ID : komh
                                                     komh@chollian.net
                                                          수정: 고명훈
