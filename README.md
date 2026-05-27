# UE5 Simple Area Manager

> Built as a lightweight area management plugin for Unreal Engine 5.

`Simple Area Manager`는 맵에 배치된 Area와 해당 Area 안에 있는 Actor를 관리하기 위한 Unreal Engine 5 플러그인입니다.

Area Actor와 Volume을 기반으로 플레이어/멤버 Actor의 현재 Area를 추적하고, Area 변경 이벤트, 연결된 Area 조회, 미니맵 표시용 데이터, PIE/Editor Utility Widget을 함께 제공합니다.

이 플러그인은 포트폴리오 목적으로 제작되었으며, 런타임 시스템과 에디터 확장 코드를 분리하여 Unreal Engine 플러그인의 기본적인 구조를 보여주는 데 중점을 두었습니다.

---

## Features

- Area Actor 기반의 구역 관리 시스템 제공
- Volume Overlap을 이용한 Area Member 자동 등록/해제
- 플레이어와 일반 Member의 Area 변경 이벤트 분리
- 현재 Area / 연결된 Area / 전체 Area 기준 조회 지원
- Area Volume을 미니맵에 표시하기 위한 위치, 크기, 회전값 반환
- Area Actor 선택 시 연결 관계와 Volume 영역을 보여주는 Editor Visualizer 제공
- PIE 상태와 에디터 맵 로드 이벤트를 Blueprint에서 사용할 수 있는 Editor Subsystem 제공
- Editor Utility Widget 기반의 Area Manager / Viewer / Minimap 예제 제공
- 샘플 콘텐츠 포함 버전과 기본 플러그인 버전 제공

---

## Why This Plugin?

게임에서 던전, 방, 전투 구역, 스테이지 섹션처럼 "현재 Actor가 어느 영역에 있는지"를 알아야 하는 경우가 자주 있습니다.

Unreal Engine의 Trigger Volume이나 Collision 이벤트만으로도 이런 기능을 만들 수 있지만, 여러 Area를 관리하고, 현재 플레이어 위치에 따른 UI 표시나 미니맵 갱신까지 연결하려면 반복적인 관리 코드가 필요합니다.

이 플러그인은 다음과 같은 목표로 제작했습니다.

- Area와 Area 안의 Actor 목록을 한 곳에서 관리하기
- Blueprint에서 현재 Area 정보를 쉽게 조회하기
- Area 변경 이벤트를 기반으로 UI나 게임 로직을 연결하기
- Area 간 연결 관계를 에디터에서 시각적으로 확인하기
- 포트폴리오용으로 Runtime 모듈과 Editor 모듈의 분리 구조를 보여주기

복잡한 월드 스트리밍, 네트워크 동기화, 대규모 공간 질의 시스템이 필요한 프로젝트라면 별도의 전용 시스템을 구성하는 것을 권장합니다.

---

## Repository Structure

이 저장소에는 두 가지 버전의 플러그인이 포함되어 있습니다.

| Folder | Description |
| --- | --- |
| `AreaManager` | 기본 플러그인 버전입니다. Runtime / Editor 코드와 공통 UI 콘텐츠가 포함되어 있습니다. |
| `AreaManager_WithSampleContents` | 샘플 맵, 샘플 캐릭터, 샘플 입력, 테스트용 리소스가 포함된 버전입니다. |

샘플 프로젝트를 바로 확인하고 싶다면 `AreaManager_WithSampleContents`를 사용하는 것을 권장합니다.

---

## Core Classes

| Class | Role |
| --- | --- |
| `AAreaManagerAreaActor` | 하나의 Area를 나타내는 Actor입니다. 연결된 Volume을 수집하고 Overlap 이벤트를 Subsystem에 전달합니다. |
| `UAreaManagerMemberComponent` | Area에 속할 수 있는 Actor에 추가하는 Component입니다. 현재 Area 이름과 아이콘 데이터를 보관합니다. |
| `UAreaManagerSubsystem` | Area Actor와 Member Actor의 등록 상태를 관리하는 World Subsystem입니다. |
| `UAreaManagerFunctionLibrary` | 미니맵 표시를 위한 Area Volume Transform 계산 함수를 제공합니다. |
| `UAreaManagerEditorSubsystem` | PIE, 맵 로드 등 에디터 이벤트를 Blueprint에서 사용할 수 있도록 제공합니다. |
| `FAreaManagerVisualizer` | Area Actor 선택 시 Area 위치, 연결 관계, Volume 영역을 Viewport에 표시합니다. |

---

## Area Range Types

Blueprint에서는 `EAreaManagerRangeType`을 통해 조회 범위를 선택할 수 있습니다.

| Type | Description |
| --- | --- |
| `All` | 등록된 모든 Area를 대상으로 조회합니다. |
| `Connected` | 현재 플레이어 Area와 직접 연결된 Area를 대상으로 조회합니다. |
| `Current` | 현재 플레이어가 위치한 Area만 대상으로 조회합니다. |

`Connected`는 직접 연결된 Area만 반환합니다. 연결 그래프 전체를 재귀적으로 탐색하지 않습니다.

---

## Installation

1. 이 저장소의 최신 릴리즈 또는 Source Code zip을 다운로드합니다.
2. 프로젝트의 `Plugins` 폴더에 원하는 버전의 플러그인 폴더를 복사합니다.
   - 기본 버전: `AreaManager`
   - 샘플 포함 버전: `AreaManager_WithSampleContents`
3. Unreal Editor를 실행합니다.
4. Plugins 창에서 `Simple Area Manager` 플러그인을 활성화합니다.
5. 프로젝트를 재시작합니다.

샘플 포함 버전은 `EnhancedInput` 플러그인을 사용합니다.

---

## Basic Usage

1. 레벨에 `AreaManagerAreaActor`를 배치합니다.
2. Area로 사용할 Volume Actor를 `AreaManagerAreaActor`에 Attach합니다.
3. `AreaName`을 설정합니다.
4. Area에 포함될 수 있는 Actor에 `AreaManagerMemberComponent`를 추가합니다.
5. 플레이를 시작하면 Area Actor가 Subsystem에 등록되고, Member Actor는 Overlap 상태에 따라 현재 Area를 갱신합니다.

Blueprint에서는 다음 함수를 사용할 수 있습니다.

| Function | Description |
| --- | --- |
| `FindAreaData` | 이름으로 Area Data를 조회합니다. |
| `GetCurrentPlayerArea` | 현재 플레이어가 속한 Area를 반환합니다. |
| `GetAreaActors` | 지정한 범위의 Area Actor 목록을 반환합니다. |
| `GetAreaMembers` | 지정한 범위에 속한 Member Actor 목록을 반환합니다. |
| `Get Area Volume Transform (for Minimap)` | Area Volume을 미니맵에 표시하기 위한 상대 위치, 월드 크기, 회전값을 반환합니다. |

---

## Editor Tools

### Area Manager Window

Toolbar 또는 Window 메뉴에서 `AreaManager`를 실행할 수 있습니다.

Editor Utility Widget을 통해 Area 관련 에디터 도구와 예제 UI를 확인할 수 있습니다.

### Area Visualizer

Viewport에서 `AreaManagerAreaActor`를 선택하면 다음 정보를 시각적으로 확인할 수 있습니다.

- 선택된 Area 위치
- 연결된 Area 위치
- Area 간 연결선
- 선택된 Area의 Volume 영역
- Area 이름 라벨

### Collect Connected Areas

`AreaManagerAreaActor`에는 에디터에서 실행할 수 있는 `Collect Connected Areas` 함수가 있습니다.

이 함수는 Volume 경계가 겹치는 Area를 찾아 `ConnectedAreas`에 추가합니다. 기존 연결은 유지되며, 개발자가 수동으로 구성한 연결을 덮어쓰지 않습니다.

---

## Recommended Use Cases

- 방 단위 던전 구성
- 전투 구역 진입/이탈 감지
- 미니맵에 현재 Area와 주변 Area 표시
- 플레이어가 위치한 Area 기준으로 UI 갱신
- Area별 NPC 또는 오브젝트 목록 조회
- 에디터에서 Area 연결 관계를 빠르게 확인하는 도구

---

## Notes

- `AreaName`은 중복되지 않게 설정하는 것을 권장합니다.
- Runtime Subsystem은 PIE와 Game World에서 동작합니다.
- Area Volume은 `AreaManagerAreaActor`에 Attach된 `AVolume`을 기준으로 수집됩니다.
- 샘플 콘텐츠는 사용 예시를 보여주기 위한 목적으로 포함되어 있습니다.

---

## Design Note

이 플러그인은 포트폴리오 목적으로 제작되었습니다.

따라서 지나치게 복잡한 추상화보다, Unreal Engine 플러그인의 기본 구조, Runtime / Editor 모듈 분리, Blueprint 사용성, 에디터 확장 흐름을 명확하게 보여주는 것을 우선했습니다.

---

## Current Status

- Portfolio project
- Runtime + Editor plugin
- Developed with Unreal Engine 5.7
- Includes optional sample contents
- MIT License

---

## License

This project is licensed under the MIT License.
