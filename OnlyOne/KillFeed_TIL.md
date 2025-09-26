# TIL: KillFeed 위젯 분석 및 구현 개요 (UE5 C++/UMG)

작성일: 2025-09-25

## TL;DR
- KillFeed는 ScrollBox(컨테이너) + 개별 항목 위젯의 조합이며, 타이머로 일정 시간 뒤 항목을 제거한다.
- 현재 구조는 동작하지만 다음을 반드시 정리 필요:
  - 항목 위젯(SetKillFeedTexts)에서 AddToViewport 호출 제거(레이어/중복 추가 문제).
  - 타이머 수명 관리 개선(객체 귀속/Weak 참조, NativeDestruct에서 정리).
  - RemoveChild와 RemoveFromParent 중복 제거 정리.
  - 에디터용 더미 타이머는 Editor/Debug 빌드 한정.

---

## 구현 개요(Overview)
- 목표: 킬/데스 이벤트가 발생할 때 화면 우측/상단 등에 일정 시간 표시 후 자동으로 사라지는 피드 제공.
- 방식: 컨테이너 위젯이 ScrollBox에 항목 위젯을 동적으로 추가하고, KillFeedDuration 경과 후 제거.
- 데이터: KillerName, VictimName(필수), 필요시 무기/팀/아이콘 등 확장 가능.

## 구성 요소(Components)
1) UPOKillFeedWidget (컨테이너)
- 보관: ScrollBox KillFeedList, 항목 클래스 KillFeedEntryClass, 표시 시간 KillFeedDuration
- 역할: 항목 생성/추가/제거 스케줄링, (선택) 자동 스크롤, 정렬 방향 제어
- 생명주기: NativeConstruct(초기화/테스트 피드), NativeDestruct(타이머 정리)

2) UPOKillFeedElementWidget (항목)
- 보관: UTextBlock* KillerText, VictimText
- 역할: 텍스트 바인딩, (선택) 페이드 아웃 애니메이션 처리
- 주의: 항목 스스로 AddToViewport를 호출하지 않는다(부모 컨테이너가 관리).

## 인터페이스 계약(Contract)
- 입력: AddKillFeedEntry(KillerName: FString, VictimName: FString)
- 전제 조건: KillFeedEntryClass != nullptr, KillFeedList != nullptr
- 정상 결과: ScrollBox에 항목이 추가되고 KillFeedDuration 이후 제거됨
- 오류 모드: 전제 조건 미충족 시 조용히 무시(로그 권장) 또는 가드 실패 처리

## 동작 흐름(Sequence)
1. NativeConstruct
- (에디터/디버그 전용) 주기적으로 더미 항목 추가하는 타이머 등록
2. AddKillFeedEntry
- 항목 위젯 생성 → 텍스트 바인딩 → ScrollBox에 AddChild(또는 InsertChildAt(0)) → 제거 타이머 등록
3. 제거 시점
- 타이머 콜백에서 ScrollBox에서 제거(필요시 애니메이션 완료 후 제거)
4. NativeDestruct
- 등록된 타이머 모두 정리

## 현재 코드 기반 관찰
- UPOKillFeedElementWidget::SetKillFeedTexts 내부에서 AddToViewport 호출됨 → 컨테이너 관리와 충돌
- 타이머 핸들을 지역 변수로만 보관, 람다에 this/NewEntry 캡처 → 파괴 순서에 따른 Dangling Pointer 위험
- RemoveChild 이후 RemoveFromParent까지 호출 → 중복 제거 호출 가능성
- 더미 데이터 타이머가 항상 활성화됨(빌드 가드 없음)

## 개선안(구체)
1) 항목 위젯의 레이어 관리
- SetKillFeedTexts에서 AddToViewport 삭제
- 자식 추가는 KillFeedWidget이 ScrollBox::AddChild/InsertChildAt으로만 수행

2) 타이머 수명/안전성
- 멤버로 FTimerHandle(또는 배열) 보관하여 NativeDestruct에서 명시적으로 ClearTimer
- 또는 FTimerDelegate + TWeakObjectPtr<UPOKillFeedElementWidget> 사용해 안전 제거
- 콜백 내에서 IsValid(Container) && IsValid(Entry) 확인 후 접근

3) 제거 로직 단일화
- 컨테이너 관점: KillFeedList->RemoveChild(Entry)만으로 충분(부모-자식 끊김)
- 항목 내부 RemoveFromParent는 컨테이너 외부에서 생성된 경우만 보조적으로 유지(현 구조에선 불필요)

4) 테스트용 타이머 가드
- #if WITH_EDITOR || !UE_BUILD_SHIPPING 등의 전처리로 에디터/디버그에서만 실행
- 또는 구성 변수(예: bEnableKillFeedDebugFeed)로 런타임 토글

5) UX/확장 옵션
- 최신 항목 상단 삽입(InsertChildAt 0), 자동 스크롤(ScrollToStart/End)
- 페이드아웃 애니메이션 후 제거(애니메이션 Finished 델리게이트에서 RemoveChild)
- UListView 활용한 대량 이벤트 성능 최적화

## 엣지 케이스
- ScrollBox/KillFeedEntryClass 미설정
- 위젯이 닫힌 직후 타이머 콜백 발화(파괴 순서 경쟁)
- 짧은 시간에 대량 이벤트(수백 개) → 프레임 드랍/메모리 증가
- 동일 항목 중복 추가 시 시각적 혼잡

## 테스트 플랜
- 기본: 이벤트 추가 후 KillFeedDuration 경과 시 정상 제거 확인(유령 위젯/누수 없음)
- 파괴 안전성: 위젯 열고 즉시 닫기 반복, 크래시 없음 확인
- 대량: 100개 연속 삽입 시 스크롤/프레임 유지
- 빌드 가드: Shipping 빌드에서 더미 타이머 미작동 확인
- UX: 상단 삽입/자동 스크롤/페이드아웃 동작 여부

## 작업 체크리스트
- [ ] SetKillFeedTexts에서 AddToViewport 제거
- [ ] 타이머 핸들 멤버화 또는 Weak Delegate로 전환, NativeDestruct에서 정리
- [ ] 제거 로직을 RemoveChild 중심으로 단일화
- [ ] 더미 타이머 전처리 가드 추가
- [ ] 선택: 상단 삽입/자동 스크롤/페이드아웃 구현

## 참고 코드 스케치(요지)
- 제거 타이머(Weak):
  - FTimerDelegate Delegate; TWeakObjectPtr<UPOKillFeedElementWidget> WeakEntry = NewEntry;
  - Delegate.BindLambda([this, WeakEntry]() { if (IsValid(this) && WeakEntry.IsValid() && KillFeedList) { KillFeedList->RemoveChild(WeakEntry.Get()); }});
  - GetWorld()->GetTimerManager().SetTimer(Handle, Delegate, KillFeedDuration, false);
- 전처리 가드:
  - #if WITH_EDITOR
  -   // 더미 타이머 등록
  - #endif

## 결론
- 작은 수정으로 안전성과 유지보수성을 크게 개선할 수 있다. 특히 레이어 관리(AddToViewport 제거)와 타이머 수명 관리부터 우선 반영하자.
