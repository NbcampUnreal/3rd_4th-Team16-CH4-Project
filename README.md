# 🎮 OnlyOne Project

**OnlyOne**은 언리얼 엔진으로 개발 중인 **멀티플레이어 3인칭 서바이벌 슈팅 게임**입니다.  
플레이어는 **최후의 1인**이 되는 것을 목표로 하며, 언리얼 엔진의 **Gameplay Ability System (GAS)** 을 기반으로  
스킬, 전투, 상호작용 등 다양한 시스템을 구현했습니다.

---

## 🧑‍💻 개발 환경
- Unreal Engine 5.5
- C++ 기반 개발  
- Dedicated Server / Client 구조
---

## 🌟 주요 기능 (Key Features)

### 🌐 데디케이티드 서버 기반 멀티플레이
- **Dedicated Server 아키텍처**를 채택하여 안정적이고 확장 가능한 환경을 구축했습니다.  
- 모든 **게임 로직은 서버에서 처리**되어 클라이언트 부담을 최소화하고 **공정한 게임 플레이**를 보장합니다.

### ▶️ 로비 시스템
- `APOLobbyGameMode`를 통해 플레이어들이 서버에 접속해 게임 시작을 준비합니다.  
- `Seamless Travel`을 통해 **로비 → 게임 스테이지**로 자연스럽게 전환됩니다.

### 🧍 플레이어 상태 동기화
- `APOLobbyPlayerState`에서 닉네임, 준비 상태, 생존 여부, 킬 수 등의 정보를 **네트워크로 실시간 동기화**합니다.

---

## 🚀 Gameplay Ability System (GAS)

- 캐릭터의 **모든 행동, 능력, 상태**를 GAS를 통해 통합 관리합니다.

### 💥 어빌리티 및 속성 관리
- `UPOAbilitySystemComponent`, `UPOAttributeSet`을 사용해 캐릭터의 **체력, 데미지, 스킬**을 제어합니다.

### 🧩 모듈식 설계
- `UPODataAsset_StartupDataBase`를 통해 캐릭터 타입(플레이어, NPC)에 따라  
  **초기 능력치, 스킬, 게임플레이 이펙트**를 유연하게 설정할 수 있습니다.

### 🔥 커스텀 데미지 계산
- `UGameplayEffectExecutionCalculation`을 상속받은  
  **`UPOGECalculation_DamageTaken` 클래스**를 통해 **커스텀 데미지 계산 로직**을 구현했습니다.  
- 공격 어빌리티(`UPOPlayerGameplayAbility_Attack`)에서 **SetByCaller** 방식으로 기본 데미지를 전달하면,  
  데미지 계산 클래스가 최종 데미지를 산출하여 **`UPOAttributeSet`의 체력 속성(Health Attribute)** 에 반영합니다.  
- 이를 통해 **상황별 보정(치명타, 방어력, 거리 보정 등)** 을 유연하게 확장할 수 있습니다.

### 💨 아이템 사용 횟수 관리
- `UPOAttributeSet`의 **`SmokeGrenadeCount`** 초기값 2 설정
- 연막탄 어빌리티(`UPOPlayerGameplayAbility_SmokeGrenade`)에서 **Cost** 로 사용
- 속성이 0이 되면 발동 불가 → **게임 내 2회만 사용 가능**

### 📈 플레이어 스탯 관리
- 캐릭터 능력치(체력, 공격력, 이동 속도 등)를 **Curve Table**로 관리
- 레벨별 곡선(Curve)을 사용해 능력치 자동 보간
- 데이터 에셋(`UPODataAsset_StartupDataBase`)에서 초기 능력치 로드
- 능력치 변화(버프/디버프)는 GAS `GameplayEffect`로 실시간 반영

#### 📄 Curve Table 예시 (`PlayerStat_CurveTable.csv`)

|  | 1 | 2 | 3 | 4 |
|:------:|:-------:|:-------:|:---------:|:-----------:|
| Player.BaseDamage        | 20.0  | 22.0  | 24.0  | 26.0  |
| Player.MaxHealth         | 100.0 | 110.0 | 120.0 | 130.0 |
| Player.SmokeGrenadeCount | 2.0   | 3.0   | 4.0   | 5.0   |

> *Curve Table은 언리얼 에디터 `Data Table`로 변환되어, GAS 초기화 시 `UPOAttributeSet`에 매핑됩니다.*
---

## ⚔️ 전투 시스템 (Combat System)

- **컴포넌트 기반 설계**를 통해 유연하고 확장 가능한 전투 구조를 구현했습니다.

#### 🧱 전투 로직 분리
- `UPawnCombatComponent`를 기반으로  
  - 플레이어용: `UPlayerCombatComponent`  
  - NPC용: `UNpcCombatComponent`  
  로 전투 로직을 분리 관리합니다.

#### 🎯 정확한 충돌 판정
- `UAN_ToggleCollision` 애니메이션 노티파이를 사용하여  
  **공격 애니메이션의 특정 시점에만 충돌 판정**이 활성화되도록 제어합니다.

---

## 🤖 인공지능 (AI)

- **Behavior Tree** 기반의 AI 캐릭터를 구현했습니다.

#### 🧍‍♂️ NPC 캐릭터
- `APONPCCharacter`는 월드를 순찰하고 플레이어와 상호작용합니다.

#### 🧭 자율 행동 태스크
- `BTT_FindRandomLocation`, `BTT_SetRandomSpeed` 등을 사용하여  
  **무작위 이동 및 속도 변경** 등 다양한 행동 패턴을 표현합니다.

---

## 🖥️ UI 시스템

- `UPOUIStackingComponent`를 활용하여 **체계적인 UI 관리 시스템**을 구현했습니다.

#### 🧱 UI 스택 관리
- 위젯을 **Stack 구조로 관리**하여 메뉴 전환 및 팝업 표시를 효율적으로 처리합니다.

#### 🧩 다양한 UI 구성
- 메인 메뉴  
- 서버 접속  
- 로비  
- 인게임 HUD  
- 설정  
- 플레이어 목록  

등 다양한 UI 위젯을 제공합니다.

---

## 🧩 상호작용 및 기믹 시스템

- 플레이어가 **게임 월드와 상호작용할 수 있는 다양한 요소**를 제공합니다.

#### 🔗 상호작용 시스템
- `IPOInteractableInterface` + `UPOInteractManagerComponent`를 사용해  
  문, 아이템 등 오브젝트와의 상호작용을 구현했습니다.

#### 🎮 게임 기믹
- 문 (`APODoor`)  
- 사운드 지뢰 (`APOSoundMine`)  
- 승리 버튼 (`APOHoldToWinButton`)  
  등 게임의 재미를 높이는 다양한 기믹이 포함되어 있습니다.

---

## 🛠️ 아키텍처 (Architecture)

### 🧭 Game Flow
`MainMenu → Lobby → Stage`  
세 단계로 구성되며, 각 단계는 **서버 전용 GameMode & GameState** 가 관리합니다.

| 단계 | 클래스 | 설명 |
|------|---------|------|
| 메인 메뉴 | `APOMainMenuGameMode` | 게임 시작 진입점 관리 |
| 로비 | `APOLobbyGameMode` | 플레이어 준비 및 매칭, 이후 Stage로 이동 |
| 스테이지 | `APOStageGameMode` | 플레이어/AI 스폰, 생존자 체크, 승리 판정 등 핵심 로직 수행 |

---

### 🧍 Character 구조
- 모든 캐릭터는 `APOCharacterBase`를 상속받으며 **GAS 관련 컴포넌트**를 포함합니다.

| 캐릭터 | 설명 |
|---------|------|
| `APOPlayerCharacter` | 플레이어 조작 캐릭터. `EnhancedInput` 시스템을 통해 입력 처리 |
| `APONPCCharacter` | Behavior Tree 기반 AI 제어 캐릭터 |

---

### 🪟 UI 구조
- 모든 UI 위젯은 `UPOBaseWidget`을 상속받아 일관성 유지  
- `UPOUIStackingComponent`가 UI 흐름과 입력 상태(UI 모드/게임 모드)를 자동으로 관리합니다.

---

## 📦 주요 모듈 (Dependencies)

### `OnlyOne.Build.cs`에 정의된 주요 언리얼 엔진 모듈
```
Core, CoreUObject, Engine
InputCore, EnhancedInput
GameplayTags, GameplayTasks, GameplayAbilities
AIModule, NavigationSystem
UMG, Slate, SlateCore
Niagara
```
---

## 📂 폴더 구조 (Folder Structure)
```
OnlyOne/
├── Public/
│ ├── AI/
│ ├── AnimationInstances/
│ ├── Characters/
│ ├── Components/
│ │ ├── Combat/
│ │ ├── Input/
│ │ ├── Interact/
│ │ └── UI/
│ ├── Controllers/
│ │ ├── Components/
│ │ └── Interfaces/
│ ├── Core/
│ ├── DataAssets/
│ │ ├── Input/
│ │ └── Startup/
│ ├── Game/
│ ├── GameAbilitySystem/
│ │ ├── Abilities/
│ │ │ ├── NPC/
│ │ │ └── Player/
│ │ └── Calculation/
│ ├── Interfaces/
│ ├── Items/
│ ├── Map/
│ │ └── Gimmick/
│ ├── Notifies/
│ ├── POTypes/
│ ├── Save/
│ └── UI/
│ ├── Common/
│ ├── HowToPlay/
│ ├── InGameMenu/
│ ├── KillFeed/
│ ├── MainMenu/
│ ├── PlayerCounter/
│ ├── PlayerStateList/
│ ├── ServerLobbyMenu/
│ ├── SettingMenu/
│ ├── SmokeCounter/
│ ├── StageTimer/
│ ├── Timer/
│ └── WinnerDecided/
└── Private/
└── ... (Public 폴더와 유사한 구조)
```

## 패키징 중 발생한 문제
패키징 중 발생한 문제에 대해서는 해당 블로그에서 확인해 보실 수 있습니다.
https://velog.io/@dev_sensational/Project-OnlyOne-Milestone-1-%EB%B9%8C%EB%93%9C-%EB%B0%8F-%EB%AC%B8%EC%A0%9C-%ED%95%B4%EA%B2%B0
