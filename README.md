# stm32-with-ds18b20
stm32 interface communicate onewire with ds18b20 

đầu tiên khởi động cho chuyên đề này ta sẽ giao tiếp với cảm biến ds18b20. 
đầu tiên hãy phân tích những điểm sau của cảm biến này.

cảm biến này sử dụng độ phân giải 9bit đến 12 bit. 
nó dùng giao tiếp 1 - wire nhớ mắc thêm điện trở cho nó nếu muốn giao tiếp, điện trở cần mắc là điện trở 4k7 ohm. 

đặc tính thứ hai là con cảm biến này có độc lập 64 bit mã seri code có thể dùng để giao tiếp với nhiều con cảm biến ds18b20 mà không bị xung đột.

Nó có cả bộ nhớ eeproom bên trong cảm biến nữa quá sức đặc biệt.

applications

thermostatic controls: kiểm soát nhiệt độ 
industrial systems: hệ thống công nghiệp 
consumer products: sản phẩm tiêu dùng
thermometers: nhiệt kế
thermally sensitive systems: nhiều hệ thống nhạy cảm nhiệt 

nhiệt độ hoạt động là -55 độ C đến 127 độ C (-67 độ F đến 257 độ F).

câu hỏi đặt ra là nonvolatile trong ds18b20 là gì ? 

là một khả năng lưu trữ dữ liệu sau khi cảm biến bị mất nguồn, giả sử mình cấu hình cảm biến để nó đọc nhiệt độ sau một sự cố gì đó khiến nó mất điện thì tất cả những cấu hình mình cấu hình cho cảm biến sẽ không bị biến mất mà lưu vào bộ nhớ không bay hơi. Nên khi có điện lại mình không cần cấu hình lại cảm biến vẫn sẽ hoạt động lại bình thường. Chỉ có tham số nhiệt độ là không tồn tại mãi vì nó sẽ thay đổi sau khi mỗi lần lấy giá trị nhiệt độ.

giải thích về chức năng cảnh báo được cất vào trong bộ nhớ không bay hơi của cảm biến ds18b20.

Cảm biến ds18b20 có khả năng lưu trữ giá trị nhiệt độ giới hạn do người dùng lập trình và thông qua lệnh command alarm search nó sẽ tìm ra các cảm biến nào có nhiệt độ đang vượt qua nhiệt độ giới hạn và gửi cảnh báo thiết bị đó về cho MCU để thực hiện xử lý tức thời.

câu hỏi thứ ba parasite power mode trong ds18b20 là gì ? 

đây là một chế độ tiết kiệm năng lượng và tiết kiệm dây nối , nó sẽ không sử dụng nguồn từ chân VDD nữa mà chân này sẽ nối xuống đất , thay vào đó nó sẽ sử dụng nguồn từ chân DATA gọi là DQ chân này sẽ vừa được dùng để xử lý dữ liệu vừa được dùng để cấp nguồn điện vì bên trong con ds18b20 này có một con diode và 1 con tụ điện nó có thể cấp nguồn cho mạch.

Nhưng lưu ý hiệu suất sẽ kém đi và cần phải có điện áp ổn định để tránh gây lỗi khi gửi dữ liệu.
Bộ nhớ scratchpad memory trong ds18b20 là gì ?

Bộ nhớ sratchpad memory trong ds18b20 là bộ nhớ tạm thời , nơi lưu trữ các giá trị đo được như nhiệt độ, và các thông tin cấu hình của cảm biến. 

bộ nhớ scratchpad trong DS18B20 có cấu trúc bao gồm 9 bytes dữ liệu. 
1. byte 0-1: đo nhiệt độ(temperature data)
- ds18b20 sử dụng độ phân giải 9 đến 12 bit, nên giá trị nhiệt độ sẽ được lưu trữ trong 2 byte này.

2. Byte 2: configuration register:
- byte này chứa thông số cấu hình, bao gồm độ phân giải và các chế độ hoạt động của cảm biến.

3. byte 3-6 : không sử dụng
4. byte 7: alarm high (giới hạn nhiệt độ cao)
byte này lưu trữ giới hạn nhiệt độ cao mà người dùng cài đặt nếu nhiệt độ đo được vượt quá giới hạn này nó sẽ gửi cảnh báo

5. byte 8 : alarm low (giới hạn nhiệt độ thấp)
tương tự như byte 7 nhưng byte này sẽ lưu trữ giới hạn nhiệt độ thấp nếu mà nhiệt độ đo được thấp hơn byte này thì cảm biến sẽ cảnh báo.

nói chúng nhiệt độ thấp hay cao thì cũng rơi vào từ 0 -> 255 




operation -measuring temperature.

9 -> 0.5 độ C
10 -> 0.25 độ C
11 -> 0.125 độ C
12 -> 0.0625 độ C

Giải thích một chút về cách đo mặc đinh khi có nguồn độ phân giải là 12 bit tương ứng sai số đo là 0.0625 độ C

để thiết lập quá trình đo nhiệt độ thì master phải gửi 0x44 mã hexa cho ds18b20

nếu ds18b20 được cung cấp nguồn bởi VDD thì mình sẽ dựa vào "read time slots" để thực hiện lấy dữ liệu. Sau khi gửi lệnh T command ds18b20 sẽ phản hổi bằng cách gửi 0 và bắt đầu temperature convert và gửi 1 khi quá trình này xong.

nếu ds18b20 được sử dụng trong chế độ ký sinh thì cách trên không dùng được vì bus lúc này được kéo lên cao.

trong 2 byte dữ liệu temperature có bit s được dùng để xác định nhiệt độ âm hay nhiệt độ dương. bit S này là 0 thì là nhiệt độ dương (positive) bit S này là 1 thì nhiệt độ âm (nagative). 
Nếu ds18b20 được cấu hình với độ phân giải là 12 bit thì tất cả các bit này đều là dữ liệu hợp lệ. Nếu cấu hình với độ phân giải 11 bit thì bit 0 là không xác đinh, nếu cấu hình độ phân giải là 10 bit thì bit 0 và 1 là không xác đinh, nếu cấu hình độ phân giải là 9 bit thì bit 0,1,2 là không xác đinh. 

giải thích quá trình hoạt động của ds18b20.

Sau khi chuyển đổi nhiệt độ được hoàn thành thì sẽ tiến hành so sánh nhiệt độ này với 2 byte nhiệt độ giới hạn cao và nhiệt độ giới hạn thấp nằm trong bộ nhớ không bay hơi eeprom của ds18b20. Và khi so sánh thì chỉ so sánh từ byte 4 đến byte 11 của thanh ghi nhiệt độ với 2 thanh ghi 8 bit này. nếu nhiệt độ bằng hoặc thấp hơn TL (nhiệt độ giới hạn thấp) hoặc bằng và cao hơn Th(nhiệt độ giới hạn cao) thì cờ flag alarm bên trong ds18b20 này sễ được set, cờ flag này sẽ được cập nhật liên tục vì vậy nếu điều kiện này biến động bình thường trở lại thì cờ flag này sẽ tắt.

Thiết bị master tức là (MCU) có thể check trạng thái cờ alarm này của tất cả các con ds18b20 trên bus, bằng cách gửi lệnh alarm search 0xECh. bất kỳ ds18b20 nào set cờ alarm sẽ phản hồi lệnh này, nên master có thể xác định chính xác ds18b20 nào để bị set cờ cảnh báo này.  Nếu điều kiện cảnh báo tồn tại nhưng thay đổi ngưỡng TH và TL, thì nên thực hiện một quá trình chuyển đổi nhiệt độ để xác thực lại cờ alarm.


alarm seach command 0xEC hexa. 






code này là để chuyển đổi 16 bit trong 2 bytes nhiệt độ sẽ nhận được từ ds18b20 thành nhiệt độ chính xác.
```c
#include <stdio.h>
#include <stdint.h>


float convertBinaryToTemperature(uint16_t data)
{
    float result;
    // if flagSign = 1 is nagative, flagSign = 0 positive
    int flagSign = 0;
    int temp = data >> 11;
    if(temp & 0x01)
    {
        flagSign = 1;
    }
    temp = (data >> 4) & 0x7F;
    if(!flagSign)
    {
        if(data & 0xF)
        {
            switch(data & 0xF)
            {
                case 1:
                {
                    result = (temp + 0.0625);
                    break;
                }
                case 2:
                {
                    result = (temp + 0.125);
                    break;
                }
                case 4:
                {
                    result = (temp + 0.25);
                    break;
                }
                case 8:
                {
                    result = (temp + 0.5);
                    break;
                }
            }
        }
        else
        {
            return temp;
        }
    }
    if(flagSign)
    {
        result = 127 - temp;
        if(temp < 100)
        {
            result +=1;
        }
        if(data & 0xF)
        {
            switch(data & 0xF)
            {
                case 8:
                {
                    result = - (result + 0.5);
                    break;
                }
                case 0b1110:
                {
                    result = - (result + 0.125);
                    break;
                }
                case 0b1111:
                {
                    result = - (result + 0.0625);
                    break;
                }
                case 0b1100:
                {
                    result = - (result + 0.25);
                    break;
                }
            }
        }
        else
        {
            return -result;
        }
    }
    return result;
}


int main(void)
{


    uint16_t testData[] = {
        0x07D0, 0x0550, 0x0191, 0x00A2, 0x0008, 0x0000, 0xFFF8, 0xFF5E, 0xFE6F, 0xFC90
    };
    for(int i = 0; i < sizeof(testData)/sizeof(uint16_t); i++)
    {
        printf("%f ",convertBinaryToTemperature(testData[i]));
    }


    return 0;
}



đây là thanh ghi của bytes giới hạn ngưỡng nhiệt độ thấp và giới hạn ngưỡng nhiệt độ cao trong ds18b20.









command copy scratchpad 0x48 
time data transfer twr = 10ms

skip rom command 0xCC
read power supply 0xB4

memory ds18b20 
byte  0 and byte 1 of scratchpad contain LSB and MSB of temperature register, these bytes are read-only.
bytes 2 and 3 provide access to TH and TL registers.
Byte 4 contains the configuration register data
byte 5,6,7  are reserved for internal use by device and cannot be overwritten.
byte 8 of the scratchpad is read-only and contains the CRC code for bytes 0 through 7 of the scratchpad.

data được viết đến byte 2 3 và 4 của scratchpad sử dụng command Write scratchpad 0x4E. 

Để xác minh tính toàn vẹn của dữ liệu scratchpad có thể đọc sử dụng command Read Scratchpad 0xBE, sau khi data được viết. 

để copy cấu hình từ scratchpad đến eeprom hãy dùng command sau, Copy ScratchPad 0x48

Data có thể load từ eeprom vào scratchpad bất kỳ thời điểm nào bằng lệnh recall E^2 [B8h] khi bắt đầu gửi lệnh recall có thể nắm bắt trạng thái thực thi vì ds18b20 sẽ gửi 0 cho master và gửi 1 cho mcu khi nó done.

CONFIGURATION REGISTER

mặc định cấu hình của byte 4 là R0 = 1 and R1 = 1 (12 bit resolution) 
bit 7 và bit 0 đến 4 không được sử dụng để cấu hình.


CRC Generation 

byte cuối cùng trong scratch pad là bytes crc được tính toán từ byte 0 đến byte 7.
crc có thể dùng để tính toán lại sau khi nhận để đảm bảo tính an toàn dữ liệu trên đường truyền.

Nhớ cấu hình chân gpio giao tiếp với chân DQ là open-drain nhé.
hiểu rồi do là open drain nên khi không hoạt động thì trạng thái default là high khi cần gửi dữ liệu mới kéo xuống.

một lưu ý hay: nếu bus được giữ ở mức low hơn 480us thì tất cả các thành phần trên bus sẽ được reset.

search ROM [F0h] alarm search [ECh].

khi khởi động master cần biết có bao nhiêu thiết bị trên bus nên sẽ thực hiện lệnh search commmand theo chu kỳ, để xác định có bao nhiêu slave. Nếu có 1 slave thì dùng simpler Read ROM [33h] 

các bước giao dịch với ds18b20.
1. Khởi tạo
2. ROM command (tiếp theo là bất kỳ trao đổi dữ liệu cần thiết nào)
3. DS18B20 function command ( tương tự trên).

Sau này là bước 2 những lệnh ROM command 

Search ROM [F0h]

dùng command này khi thiết bị ds18b20 được cấp nguồn nó sẽ dùng để tìm kiếm có bao nhiêu slave cần giao tiếp.

Read ROM [33h]

cái command này được sử dụng khi ngay sau khi biết là thiết bị chỉ có 1 slave, nó cho phép master đọc 64-bit ROM code mà không cần sử dụng search ROM, nhưng nếu có nhiều thiết bị trên bus thì không dùng được bởi vì sẽ gây xung đột dữ liệu.

Match ROM [55h]

the match ROM command theo sau bởi 64-bit ROM code cho phép bus master giao tiếp với thằng slave có đúng địa chỉ. Những slave khác sẽ chờ xung reset.

Skip Rom [CCh]
master có thể gửi đến tất cả thiết bị đồng thời mà không cần gửi ROM infor. 
ví dụ master có thể yêu cầu tất cả slave gửi đồng thời nhiệt độ bằng cách:
gửi command Convert T[44h] sau đó gửi Skip Rom [CCh].
nhầm gửi skip rom [cch] sau đó gửi command convert t [44h]. để đọc nhiệt độ từ ds18b20 từ scratchpad.


note: có thể dùng một phương pháp này đối với bus chỉ có 1 slave :
dùng skip rom command [cch] sau đó dùng read scratchpad [beh] command để đọc từ slave mà không cần gửi thông tin 64 bit rom code. nhưng mà chỉ dùng đối với bus có 1 slave.


Alarm Search [Ech]

lênh này tương tự như lệnh search rom command nhưng khác mỗi chỗ chỉ có thiết bị ds18b20 nào đã set alarm flag mới phản hồi mà thôi. Mỗi chu kỳ lệnh alarm search master phải quay trở lại step 1 khởi tạo để tiếp tục.

step 3 ds18b20 function commands

Convert T [44h]

Write scratchpad [4eh]

command này dùng để ghi vào 3 byte, byte TH, byte TL , byte configure, truyền byte với bit có  trọng số thấp nhất trước, tất cả các bytes này phải được viết trước khi reset nếu không sẽ xảy ra corrupted. 

Read Scratchpad [BEh]

command này cho phép master đọc các byte trong scratchpad bắt đầu từ bit có trọng số thấp nhất trong byte 0 đến byte cuối cùng là crc. master có thể chấm dứt đọc bất cứ lúc nào nếu chỉ muốn lấy những byte cần thiết.

Copy Scratchpad [48h]

master gửi lệnh này để copy những phần cấu hình từ byte2 ,3,4 của scratchpad vào eeproom. Nêu đang sử dụng chế độ ký sinh thì ngay sau khi gửi lệnh này master cần phải kéo lên pull-up thật mạnh mẽ ít nhất 10ms nhé.

Recall EE [B8h]

lệnh này dùng để gọi lại các giá trị đã cấu hình trong byte2,3,4 trong eeprom, trạng thái của ds18b20 khi gọi recall là 0 đang trong quá trình recall 1 recall done. khi có điện lại sau khi mất điện thì recall sẽ được gọi tự động để config cho scratchpad.

Read Power Supply [B4h]

master phát ra cái lệnh này đến tất cả slave để biết xem chế độ power -mode của các slave là chế độ gì nếu em slave nào chơi mode ký sinh thì sẽ kéo xuống thấp trên bus , còn em nào chơi nguồn ngoài external thì duy trì mức cao.









Muốn reset ds18b20 kéo xung bus trong thời gian minumum là 480 us trên đường truyền.

Để tạo write 1 time-slot master phải kéo bus xuống và thả ra trong vòng 15us thì lúc đó điện trở kéo lên sẽ kéo bus lên cao lúc đó ds18b20 sẽ tự động ghi 1.

để tạo write 0: 
phải giữ bus low trong suốt thời gian ít nhất là 60us thì, thì DS18B20 sẽ tự động ghi 0.


Cách DS18B20 xác định giá trị ghi:

DS18B20 sẽ đọc trạng thái của bus trong  vòng 15us-60us,
nếu trong quá trình này nó đọc bus được là 0 thì nó ghi 0
nếu trong quá trình này nó đọc bus được là 1 thì nó  ghi 1

cho nên mới nói nếu muốn ghi 1 thì phải thả bus ra trước 15us.


quá trình khởi tạo bắt đầu bằng xung reset:
master kéo bus 1-wire xuông low trong tối thiểu 480us, khi ds18b20 phát hiện sự thay đổi từ low sang high (tức là rising edge khi master thả bus), ds18b20 sẽ đợi 15us đến 60us. 

sau khoảng thời gian này ds18b20 sẽ phát 1 xung gọi là "presence" bằng  cách kéo bus xuống thấp trong khoảng từ 60us đến 240us.

master phải tạo thời gian đọc (read time slots):

- ds18b20 chỉ có thể truyền dữ liệu cho master khi master phát lệnh read time slots. Điều này có nghĩa là sau khi master gửi lệnh như Read Scratchpad [Beh] hoặc read power supply [b4h], master cần tạo thời gian đọc ngay lập tức để ds18b20 có thể gửi dữ liệu.

- ngoài ra, sau khi master gửi lệnh convert T [44h] (lệnh chuyển đổi nhiệt độ) hoặc recall ee [b8h] (lệnh khôi phục dữ liệu từ bộ nhớ e2), master cũng cần tạo thời gian đọc để kiểm tra các tình trạng của các hoạt động này.

master kéo bus xuống 1us -> thả bus lên -> ds18b20 truyền dữ liệu (giữ high cho logic 1 hoặc kéo low cho logic 0) trong vòng 60us.

master phải đọc trạng thái của bus trong 15us đầu của khoảng thời gian này để lấy dữ liệu chính xác.

giải thích cách ghi write time slot trong ds18b20 giao tiếp với mcu.

thời gian của mỗi write time slot sẽ là 60us 
nếu trong thời gian này muốn ghi 1 vào ds18b20 thì phải kéo bus xuống mức 0 trong vòng dưới 15us sau đó thả ra để điện trở kéo , kéo nó về lại high.
nếu trong thời gian này muốn ghi 0 vào ds18b20 thì phải kéo bus xuống mức 0 trong vòng tối thiểu 60us để ds18b20 nhận biết là bạn muốn ghi 0 vào nó.

Khi nào thì ds18b20 truyền dữ liệu ? 

ds18b20 chỉ có thể truyền dữ liệu cho master khi master phát sinh ra các read time slots. Điều này có nghĩa là sau khi master gửi các lệnh đọc như Read Scratchpad [BEh] hoặc Read Power Supply [B4h], master phải tạo ra các read time slots để ds18b20 gửi dữ liệu theo yêu cầu.

ngoài các lệnh đọc dữ liệu này master cũng sẽ phát read time slot đối với các lệnh sau để ds18b20 gửi lại theo yêu cầu:
- convert T [44h]: để kiểm tra quá trình chuyển đổi nhiệt độ
- Recall E2 [B8h]: để kiểm tra kết quả của quá trình khôi phục bộ nhớ.

Cách tạo read time slots:

- Mỗi read time slots có thời gian tối thiểu là 60us
- để tạo một read time slot mcu phải kéo bus 1 wire xuống low trong vòng 1us rồi thả lên
- khi bus được thả lên ds18b20 sẽ gửi dữ liệu đến. 

3. cách ds18b20 truyền dữ liệu 
- ds18b20 sẽ truyển logic 1 bằng cách giữ bus high
- ds18b20 sẽ truyền logic 0 bằng cách kéo bus low 
sau khi ds18b20 kéo xuống bus low nó không thể kéo lên high được vì đang là open-drain nên là nó sẽ đợi đến cuối time slot của read time slot rồi thả ra để điện trở kéo lên kéo lên mức logic high.

4. thời gian hợp lệ của dữ liệu 
- dữ liệu của ds18b20 sẽ hợp lệ trong khoảng thời gian 15us khi (falling edge diễn ra)
- sau khi master phát sinh read-time slot master phải đọc dữ liệu trong thời gian 15us khi read time slot phát ra.
5. yêu cầu về thời gian (TINIT, TRC, và TSAMPLE).

TINIT (thời gian khởi tạo slot), TRC(thời gian phục hồi), TSAMPLE( thời gian đọc mẫu) cần nhỏ hơn 15us tổng cộng.
để tối đa độ chính xác TINIT và TRC càng ngắn càng tốt.
TSAMPLE(thời gian mẫu) của master nên được đặt vào cuối khoảng thời gian 15us để đảm bảo dữ liệu ổn định và chính xác nhất khi đọc.


giải thích thêm 1 read time slot được sử dụng để đọc 1 bit duy nhất từ ds18b20 , và đọc nó trong 15us sau khi kéo xuống bus low 1us rồi thả ra , thời gian của một read time slot là 60us. Nếu muốn đọc nhiều bit như trong read Scratchpad thì cần phải khởi tạo nhiều read time slot.

ví dụ nếu có một chân gpio cấu hình output dùng để kéo xuống mức thấp thì sau một khoảng thời gian mình không muốn kéo xuống mức thấp nữa thì phải làm sao nhưng sẽ không kéo nó lên mức cao chỉ đơn giản là ngưng kéo xuống mức thấp thôi. 

answer: chuyển chế độ từ output sang input. 











