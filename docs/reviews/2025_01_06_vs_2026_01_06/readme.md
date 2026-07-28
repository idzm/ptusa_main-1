# Обзор: 2025-01-06 vs 2026-01-06

## Сравнение в целом

| Период | Коммитов в master | Слитых PR | Крупнейшие зоны изменений |
|---|---|---|---|
| 2025-01-01..2025-07-28 | 101 | 99 | `PAC` (198 файлов), `test` (58), `deps` (42), `common` (29), `CMakeLists.txt` (18), `.github` (11) |
| 2026-01-01..2026-07-28 | 125 | 125 | `PAC` (240), `test` (109), `.github` (89), `deps` (26), `common` (23), `CMakeLists.txt` (18) |

**Вывод:** в 2026 году объём изменений вырос (+24% коммитов, +26% PR), но состав сместился в сторону `CI/security/workflow`, `SDK/build` и `эмулятора/устойчивости`. В 2025 году было относительно больше работы над ядром управления и аппаратным поведением.

## Что изменилось с 2026-01-01

### 1) CI / сборка / безопасность стали основным потоком работ

Это самый заметный сдвиг 2026 года. Три наиболее правившиеся файла за период — это workflow:

- `.github/workflows/cmake.yml:1-220`
- `.github/workflows/codeql-analysis.yml:12-105`
- `.github/workflows/sonar_build.yml:1-143`

Значимые слияния:

- [PR #1380](https://github.com/savushkin-r-d/ptusa_main/pull/1380) (`dev_sdk_2026`, merged 2026-07-24, commit [`2e36bddd`](https://github.com/savushkin-r-d/ptusa_main/commit/2e36bddd)) добавил поддержку AXCF2152 2026 LTS и расширил матрицу `CI/build`; текущая матрица теперь включает варианты 2024 и 2026 LTS в `.github/workflows/cmake.yml:89-140` и соответствующие пресеты в `CMakePresets.json:78-175`.
- [PR #1372](https://github.com/savushkin-r-d/ptusa_main/pull/1372) (`fix_checkout_security`, merged 2026-07-06, commit [`f2a0add4`](https://github.com/savushkin-r-d/ptusa_main/commit/f2a0add4)) включил unsafe PR checkout для CI; видно в `.github/workflows/cmake.yml:148-159` и `.github/workflows/sonar_build.yml:57-63`.
- [PR #1379](https://github.com/savushkin-r-d/ptusa_main/pull/1379) (`dev_sonar_config`, merged 2026-07-15, commit [`ca20e722`](https://github.com/savushkin-r-d/ptusa_main/commit/ca20e722)) исключил demo и вендорный код из Sonar; см. `sonar-project.properties:10-12`.
- Повторные обновления CodeQL/action шли весь год; текущая выделенная CMake-пайплайн CodeQL находится в `.github/workflows/codeql-analysis.yml:48-105`.

Это также видно по меткам: в 2026 году слитые PR содержали 56 меток `dependencies` и 37 меток `github_actions`.

### 2) Устойчивость выполнения и поддержка эмулятора/рабочего стола выросли

Несколько PR 2026 года улучшили поведение при отсутствии I/O, деградации или эмуляции:

- [PR #1181](https://github.com/savushkin-r-d/ptusa_main/pull/1181) (`copilot/fix-debugger-device-issues`, merged 2026-01-12, commit [`51543073`](https://github.com/savushkin-r-d/ptusa_main/commit/51543073)) исправил запись свойств устройств в режиме debug/emulator. Текущие кодовые пути: `PAC/common/device/device.cpp:179-189`, `2167-2259`, `2361-2366`; покрытие тестами в `test/device/PAC_dev_tests.cpp:740-748`, `5586-5663`, `5881-5883`.
- [PR #1193](https://github.com/savushkin-r-d/ptusa_main/pull/1193) (`copilot/add-error-indication-tags`, merged 2026-04-03, commit [`9a2017db`](https://github.com/savushkin-r-d/ptusa_main/commit/9a2017db)) добавил системный показ ошибок узлов/watchdog; см. `PAC/common/PAC_info.cpp:49-81`, `187-199`, а также объявления в `PAC/common/PAC_info.h:117-146` и `179-191`.
- [PR #1196](https://github.com/savushkin-r-d/ptusa_main/pull/1196) (`copilot/check-node-availability`, merged 2026-04-07, commit [`6e222c61`](https://github.com/savushkin-r-d/ptusa_main/commit/6e222c61)) добавил проверки доступности узлов для `DO/AO`; интерфейс находится в `PAC/common/bus_coupler_io.h:172-192`.
- [PR #1311](https://github.com/savushkin-r-d/ptusa_main/pull/1311) (`copilot/fix-set-cmd-behavior`, merged 2026-05-05, commit [`05edebd1`](https://github.com/savushkin-r-d/ptusa_main/commit/05edebd1)) предотвратил ложное включение простоя при `STOP/PAUSE`; см. `PAC/common/tech_def.cpp:131-166` и тесты в `test/tech_def_tests.cpp:166-214`.

### 3) OPC UA и CLI не просто добавили — их уточнили

2026 продолжил ранние работы по OPC UA и сделал их более зрелыми:

- [PR #1279](https://github.com/savushkin-r-d/ptusa_main/pull/1279) (`dev_OPC_UA_control`, merged 2026-04-07, commit [`9644e6df`](https://github.com/savushkin-r-d/ptusa_main/commit/9644e6df)) переработал CLI-аргументы и консолидировал режимы OPC UA. Текущая обработка CLI находится в `PAC/common/prj_mngr.cpp:41-147`, путь применения OPC-режима — в `:184-240`.
- [PR #1295](https://github.com/savushkin-r-d/ptusa_main/pull/1295) (`fix_OPC_to_non_block`, merged 2026-04-17, commit [`e6aeb40b`](https://github.com/savushkin-r-d/ptusa_main/commit/e6aeb40b)) сделал сервер OPC UA неблокирующим; см. `PAC/common/OPCUAServer.cpp:219-221`.

### 4) Июль 2026 закончился серьёзным обслуживанием, а не всплеском новых функций

Самый большой однодневный пик был 2026-07-06: 10 коммитов, почти все — обновления зависимостей/воркфлоу и исправление checkout. Последняя неделя окна тоже была интенсивно обслуживающей:

- 2026-07-24: [PR #1380](https://github.com/savushkin-r-d/ptusa_main/pull/1380) — поддержка SDK
- 2026-07-27: обновления actions + [PR #1390](https://github.com/savushkin-r-d/ptusa_main/pull/1390) — исправление опечаток в тестах (последний коммит `e7112d4`)

## Сравнение с тем же периодом 2025 года

### 1) 2025 был более ориентирован на функциональность ядра PAC/устройств

В 2025 году изменения больше были сосредоточены на `runtime/device` логике, а не на GitHub workflow.

Примеры значимых сливов:

- [PR #835](https://github.com/savushkin-r-d/ptusa_main/pull/835) (`fix_memory_leaks`, merged 2025-01-24, commit [`103387ed`](https://github.com/savushkin-r-d/ptusa_main/commit/103387ed)) исправил утечки памяти в OPC UA сервере, затронув `PAC/common/OPCUAServer.cpp`, `common/lua_manager.cpp` и несколько тестов.
- [PR #843](https://github.com/savushkin-r-d/ptusa_main/pull/843) (`dev_cmd_opc`, merged 2025-02-03, commit [`aa83ff0e`](https://github.com/savushkin-r-d/ptusa_main/commit/aa83ff0e)) ввёл CLI-запуск OPC UA; позже это превратилось в более широкий текущий CLI-сервис в `PAC/common/prj_mngr.cpp:41-147`.
- [PR #911](https://github.com/savushkin-r-d/ptusa_main/pull/911) (`alg_io_up`, merged 2025-05-22, commit [`987c6b46`](https://github.com/savushkin-r-d/ptusa_main/commit/987c6b46)) модернизировал опрос I/O-узлов, затронув `PAC/common/bus_coupler_io.h`, `PAC/common/uni_bus_coupler_io.cpp`, `PAC/common/tcp_cmctr.cpp` и платформенные коммуникаторы.
- [PR #907](https://github.com/savushkin-r-d/ptusa_main/pull/907) (`dependabot/submodules/deps/open62541/open62541-cae846b`, merged 2025-04-17, commit [`e17ce23b`](https://github.com/savushkin-r-d/ptusa_main/commit/e17ce23b)) обновил `open62541`; эта подсистема остаётся центральной в `CMakeLists.txt:106`.
- [PR #981](https://github.com/savushkin-r-d/ptusa_main/pull/981) (`dev_cmake_presets`, merged 2025-07-16, commit [`1e76100a`](https://github.com/savushkin-r-d/ptusa_main/commit/1e76100a)) добавил конфиги AXCF3152 2024 LTS, видимые в текущих пресетах/матрице в `CMakePresets.json:127-175` и `.github/workflows/cmake.yml:116-140`.
- [PR #989](https://github.com/savushkin-r-d/ptusa_main/pull/989) (`copilot/fix-14335de9-837b-473f-868f-b857431ac86d`, merged 2025-07-24, commit [`14e4e2f1`](https://github.com/savushkin-r-d/ptusa_main/commit/14e4e2f1)) затронул путь парсинга датчика давления в `PAC/common/device/device.cpp:3347-3491`.

Самые правившиеся негenerated файлы в 2025 году:

- `test/device/PAC_dev_tests.cpp`
- `CMakeLists.txt`
- `PAC/common/device/device.cpp`

Это соответствует году, ориентированному на поведение устройств, тесты и сборочную интеграцию.

### 2) В 2025 году CI/build всё ещё были, но вторичны

Ранние вехи workflow:

- [PR #838](https://github.com/savushkin-r-d/ptusa_main/pull/838) (`fix_sonar`, merged 2025-01-21, commit [`3927cb31`](https://github.com/savushkin-r-d/ptusa_main/commit/3927cb31)) исправил поведение SonarQube сборки.
- [PR #842](https://github.com/savushkin-r-d/ptusa_main/pull/842) добавил установку valgrind.
- [PR #841](https://github.com/savushkin-r-d/ptusa_main/pull/841) добавил проверки тестов с помощью valgrind.
- В `2025-04-21` был всплеск из 5 коммитов CMake/config ([#908](https://github.com/savushkin-r-d/ptusa_main/pull/908), [#909](https://github.com/savushkin-r-d/ptusa_main/pull/909), [#913](https://github.com/savushkin-r-d/ptusa_main/pull/913), [#914](https://github.com/savushkin-r-d/ptusa_main/pull/914), плюс поддержку датчиков [#910](https://github.com/savushkin-r-d/ptusa_main/pull/910)).

Но `.github` в 2025 трогали всего 11 раз против 89 в 2026.

### 3) Паттерн активности 2025 года был ровнее и человечнее

- Месячные коммиты 2025: Янв 10, Фев 10, Мар 19, Апр 18, Май 13, Июн 10, Июл 21.
- 2026 слитые PR были более сконцентрированы: Апр 30, Июл 27, Мар 23, с лишь одним PR в январе и двумя в феврале.

Смена авторства:

- 2025 коммиты: Dzmitry Ivaniuk 67, Dependabot 24, Copilot 2.
- 2026 коммиты: Dependabot 69, Dzmitry Ivaniuk 32, Copilot 21.

Так что 2026 был не просто «больше работы», а больше автоматизированной работы.

### Значимые всплески и аутлайеры

- `2025-07-16`: 5 коммитов, преимущественно релиз/конфиг + PLCnext LTS работа ([#978](https://github.com/savushkin-r-d/ptusa_main/pull/978)–[#981](https://github.com/savushkin-r-d/ptusa_main/pull/981)) и обработка ошибок CIP ([#975](https://github.com/savushkin-r-d/ptusa_main/pull/975)).
- `2025-04-21`: 5 коммитов, в основном настройка CMake/зависимостей.
- `2026-07-06`: 10 коммитов, почти все — поддержка workflow и зависимостей.
- `2026-04`: самый тяжёлый месяц слияний, включая долгоживущие ветки Copilot:
  - [PR #1193](https://github.com/savushkin-r-d/ptusa_main/pull/1193) создан 2026-01-31, слит 2026-04-03
  - [PR #1196](https://github.com/savushkin-r-d/ptusa_main/pull/1196) создан 2026-01-31, слит 2026-04-07
  - [PR #1195](https://github.com/savushkin-r-d/ptusa_main/pull/1195) создан 2026-01-31, слит 2026-04-14

### Две оговорки по «сырым» объёмам

- Число строк 2025 сильно раздуто коммитом `0685d557` / [PR #980](https://github.com/savushkin-r-d/ptusa_main/pull/980) (нормализация конца строки): 121 046 изменённых строк, в основном нефункционально.
- Самый высокий churn 2026 — коммит `44a6827e` / [PR #1195](https://github.com/savushkin-r-d/ptusa_main/pull/1195) (4 796 строк), большая часть — устаревшие заголовки RFID, а не новая функциональность.

## Общая оценка

### 2026 vs 2025

- Больше объёма: да.
- Больше инноваций продукта: лишь умеренно; было больше правок `PAC/test`, но настоящий рост — в `workflow/security/build` автоматизации.
- Больше операционной жёсткости: да — эмуляторный режим, доступность узлов, `watchdog/системный статус`, `CLI/OPC UA`.
- Больше автоматизации доставки: однозначно да — Dependabot и Copilot в 2026 занимают значительно большую долю.
