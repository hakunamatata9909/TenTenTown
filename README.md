# 📌TenTenTown

4인이 함께 구조물을 설치하고 적의 웨이브로부터 코어를 지키는 협동 타워 디펜스 게임입니다.
Dedicated Server 환경에서 동작하며, 모든 웨이브를 버티면 승리합니다.

> C++ 코드 검토를 위한 소스 중심 포트폴리오 저장소입니다.

[![Notion Portfolio](https://img.shields.io/badge/Notion%20Portfolio-black?style=for-the-badge&logo=notion&logoColor=white)](https://app.notion.com/p/TenTenTown-2f5beb956462815c98f5d8fb36ed21a0)
[![Team Repository](https://img.shields.io/badge/Team%20Repository-181717?style=for-the-badge&logo=github&logoColor=white)](https://github.com/NBcampUnrealTrack/3rd_4th-Team10-CH6-Project)

## 📌 시연 영상

<div align="center">
  <a href="https://youtu.be/iDdOEy3eLyI">
    <img src="https://img.youtube.com/vi/iDdOEy3eLyI/hqdefault.jpg"
         width="720"
         alt="TenTenTown 시연 영상" />
  </a>
  <br />
  <sub>이미지를 클릭하면 YouTube로 이동합니다.</sub>
</div>

## 📌 개발 환경

- 엔진: Unreal Engine 5.5
- 언어·기술: C++ / GAS / Replication / UMG / MVVM
- 네트워크: Dedicated Server
- 개발 도구: JetBrains Rider / Visual Studio 2022
- 버전 관리: Git / GitHub

## 📌 담당 역할

팀 프로젝트에서 **공통 캐릭터 구조와 Fighter·Archer 전투 구현**을 담당했습니다.

- PlayerState 소유 ASC를 활용한 서버·클라이언트 캐릭터 초기화
- Gameplay Event와 TargetData 기반 Fighter 콤보 전투
- Archer 차지 공격과 서버의 화살 발사 처리
- Arrow Rain의 풀링과 Seed 기반 연출 재현
- Gameplay Tag 차단 및 콤보 몽타주 복제 문제 해결
- 타워 상호작용과 코인 루팅 컴포넌트 구현

## 📌 핵심 C++ 코드

- **공통 캐릭터 · BaseCharacter**

  서버와 클라이언트에서 PlayerState 소유 ASC를 초기화합니다.
  Ability 등록과 입력 처리를 공통으로 관리합니다.

  [캐릭터 초기화](./Source/TenTenTown/Character/Characters/Base/BaseCharacter.cpp) · [ASC 복제 설정](./Source/TenTenTown/Character/PS/TTTPlayerState.cpp)

- **태그 차단 · BaseGameplayAbility**

  공통 Ability의 태그 판정을 보완해 상위 태그로 하위 Ability를 차단합니다.

  [태그 판정 코드](./Source/TenTenTown/Character/GAS/BaseGA/BaseGameplayAbility.cpp)

- **근접 콤보 · Fighter**

  Gameplay Event로 콤보 입력 구간과 몽타주 전환을 제어합니다.
  TargetData를 활용해 타격 대상을 처리합니다.

  [콤보 처리 코드](./Source/TenTenTown/Character/GAS/GA/Fighter/NormalAttack/GA_FighterNormalAttack.cpp)

- **차지 공격 · Archer**

  입력 유지 시간으로 차지 비율을 계산합니다.
  입력을 해제하면 서버에서 조준 방향으로 화살을 발사합니다.

  [차지 공격 코드](./Source/TenTenTown/Character/GAS/GA/Archer/NormalAttack/GA_ArcherNormalAttack.cpp)

- **광역 궁극기 · Arrow Rain**

  화살과 피격 연출을 풀에서 재사용합니다.
  Seed를 공유해 스폰 위치·방향의 난수열을 재현하고, 데미지는 서버에서 처리합니다.

  [궁극기 제어](./Source/TenTenTown/Character/GAS/GA/Archer/Ultimate/ActorArrowRain.cpp) · [화살 풀링](./Source/TenTenTown/Character/Characters/Archer/ArrowRainPooling/Arrow_ObjectPooling.cpp)

- **상호작용 컴포넌트**

  타워 상호작용과 코인 루팅을 별도 컴포넌트로 분리했습니다.

  [타워 상호작용](./Source/TenTenTown/Character/InteractionSystemComponent/InteractionSystemComponent.cpp) · [코인 루팅](./Source/TenTenTown/Character/CoinLootComponent/CoinLootComponent.cpp)

## 📌 트러블슈팅 및 설계 사례

1. **태그 계층 차단 문제**

   **문제:** 상위 태그를 차단해도 하위 태그를 가진 Ability가 실행되었습니다.

   **해결:** 엔진 내부의 태그 비교 방향을 확인했습니다. 공통 Ability의 판정 함수를 오버라이딩해 하위 태그도 차단하도록 보완했습니다.

   [태그 판정 코드](./Source/TenTenTown/Character/GAS/BaseGA/BaseGameplayAbility.cpp)

2. **콤보 몽타주 복제 지연**

   **문제:** 일부 클라이언트에서 콤보의 중간 타격 모션이 누락되었습니다.

   **해결:** ASC를 소유한 PlayerState의 복제 주기를 조정하고, 콤보 전환 시 `ForceReplication()`을 호출하도록 수정했습니다.

   [PlayerState 설정](./Source/TenTenTown/Character/PS/TTTPlayerState.cpp) ·
   [콤보 처리](./Source/TenTenTown/Character/GAS/GA/Fighter/NormalAttack/GA_FighterNormalAttack.cpp)

3. **Arrow Rain 생성·복제 비용 설계**

   **설계 과제:** 다수의 화살을 사용하는 궁극기에서 반복 생성과 개별 이동 복제 부담을 줄이고자 했습니다.

   **해결:** 화살과 피격 연출을 풀에서 재사용하고, 이동 복제 대신 Seed를 공유했습니다. 데미지는 서버에서 처리하며, 전용 서버에서는 피격 연출 생성을 생략했습니다. Seed 공유는 스폰 위치·방향의 난수열을 맞추며, 생성 시점이나 충돌 결과까지 보장하지는 않습니다.

   [궁극기 제어](./Source/TenTenTown/Character/GAS/GA/Archer/Ultimate/ActorArrowRain.cpp) ·
   [화살 풀링](./Source/TenTenTown/Character/Characters/Archer/ArrowRainPooling/Arrow_ObjectPooling.cpp)
