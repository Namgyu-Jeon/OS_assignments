# 오류 입력 실행 안내

오류 처리를 확인하기 위해 잘못된 입력을 원인별 파일로 작성했습니다. 각 파일의 오류 원인과 실행 명령, 확인한 메시지를 아래에 정리했습니다.

모든 파일은 오류 메시지를 출력하고 종료 코드 1로 끝나는 것을 확인했습니다. 서로 다른 입력에서 같은 메시지가 나올 수 있습니다. 예를 들어 음수·문자·소수는 같은 시간 값 검사에서 거부합니다.

## 파일별 오류 원인

| 파일 | 확인하는 오류 | 잘못된 입력 |
|---|---|---|
| [error.txt](error.txt) | 마지막 CPU burst 누락 | `P00	ARRIVAL:0	CPU:6	I/O:2	CPU:3 / P01	ARRIVAL:2	CPU:4	I/O:1` |
| [error_cpu_order.txt](error_cpu_order.txt) | I/O burst 없이 CPU burst가 연속해서 나오는 오류 | `P00 ARRIVAL:0 CPU:4 CPU:2` |
| [error_first_io.txt](error_first_io.txt) | 첫 burst가 I/O burst인 오류 | `P00 ARRIVAL:0 I/O:2 CPU:4` |
| [error_negative_time.txt](error_negative_time.txt) | 음수 시간 | `P00 ARRIVAL:0 CPU:-3` |
| [error_text_time.txt](error_text_time.txt) | 숫자 대신 문자 | `P00 ARRIVAL:0 CPU:abc` |
| [error_decimal_time.txt](error_decimal_time.txt) | 정수 대신 소수 | `P00 ARRIVAL:0 CPU:1.5` |
| [error_missing_arrival.txt](error_missing_arrival.txt) | 도착 시각 항목 누락 | `P00` |
| [error_missing_cpu.txt](error_missing_cpu.txt) | CPU burst 누락 | `P00 ARRIVAL:0` |
| [error_wrong_label.txt](error_wrong_label.txt) | CPU 항목 이름의 대소문자 오류 | `P00 ARRIVAL:0 cpu:4` |
| [error_colon_space.txt](error_colon_space.txt) | 콜론과 숫자 사이에 공백 | `P00 ARRIVAL:0 CPU: 4` |
| [error_empty.txt](error_empty.txt) | 내용이 없는 빈 파일 | 빈 파일(0바이트) |
| [error_integer_overflow.txt](error_integer_overflow.txt) | 입력 정수가 64비트 범위 초과 | `P00 ARRIVAL:0 CPU:9223372036854775808` |
| [error_time_overflow.txt](error_time_overflow.txt) | 도착 시각과 CPU burst time의 합이 범위 초과 | `P00 ARRIVAL:9223372036854775807 CPU:1` |

`error_integer_overflow.txt`는 입력 숫자 자체가 허용 범위를 넘습니다. `error_time_overflow.txt`는 각각의 숫자를 읽을 수 있지만, 도착 시각에 CPU burst time을 더한 결과가 범위를 넘습니다.

빈 파일 검사에는 크기가 0바이트인 `error_empty.txt`를 사용했습니다.

## 실행 명령

실행 파일과 입력 파일이 함께 있는 폴더에서 아래 명령을 **한 줄씩** 실행합니다.

```powershell
.\sjf.exe error.txt
.\sjf.exe error_cpu_order.txt
.\sjf.exe error_first_io.txt
.\sjf.exe error_negative_time.txt
.\sjf.exe error_text_time.txt
.\sjf.exe error_decimal_time.txt
.\sjf.exe error_missing_arrival.txt
.\sjf.exe error_missing_cpu.txt
.\sjf.exe error_wrong_label.txt
.\sjf.exe error_colon_space.txt
.\sjf.exe error_empty.txt
.\sjf.exe error_integer_overflow.txt
.\sjf.exe error_time_overflow.txt
```

저장소의 `project` 폴더에서 실행할 때는 `.\sjf.exe` 대신 `.\bin\x64\Release\sjf.exe`를 사용합니다.

## 예상 오류 메시지

| 입력 파일 | 실제 확인한 메시지 |
|---|---|
| `error.txt` | `Error: A burst sequence must start and end with CPU.` |
| `error_cpu_order.txt` | `Error: Unexpected input field; expected I/O:` |
| `error_first_io.txt` | `Error: Unexpected input field; expected CPU:` |
| `error_negative_time.txt` | `Error: Time values must be nonnegative 64-bit integers.` |
| `error_text_time.txt` | `Error: Time values must be nonnegative 64-bit integers.` |
| `error_decimal_time.txt` | `Error: Time values must be nonnegative 64-bit integers.` |
| `error_missing_arrival.txt` | `Error: A process is missing its arrival time.` |
| `error_missing_cpu.txt` | `Error: A burst sequence must start and end with CPU.` |
| `error_wrong_label.txt` | `Error: Unexpected input field; expected CPU:` |
| `error_colon_space.txt` | `Error: Time values must be nonnegative 64-bit integers.` |
| `error_empty.txt` | `Error: The input file contains no processes.` |
| `error_integer_overflow.txt` | `Error: Time values must be nonnegative 64-bit integers.` |
| `error_time_overflow.txt` | `Error: The simulation time exceeds the 64-bit range.` |

## 파일로 만들지 않는 오류

존재하지 않는 파일 이름을 인자로 지정하면 파일 열기 오류를 확인할 수 있습니다. 아래는 `erroe.txt`가 없는 경우의 실행 명령입니다.

```powershell
.\sjf.exe erroe.txt
```

인수 개수 오류는 파일 내용이 아닌 실행 명령으로 재현합니다.

```powershell
.\sjf.exe
.\sjf.exe input1.txt input2.txt
```

두 경우에는 `Usage: sjf.exe <input file>`을 출력합니다. 프로그램 소스나 실행 파일을 바꾸지 않고 기존 오류 처리를 확인하는 입력입니다.
