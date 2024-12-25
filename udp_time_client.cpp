//
// 情報通信応用実験 ネットワークプログラミング
//
// 首都大学東京 システムデザイン学部 情報通信システムコース
// 准教授・酒井和哉
// ２０１５年２月５日
//
// 情報科学科
// 助教・柴田祐樹
// ２０１９年１０月　改訂
// ２０２０年１０月　改訂
//

#include <arpa/inet.h>
#include <iostream>
#include <chrono>
#include <ctime>
#include <unistd.h> // https://linux.die.net/man/2/read
#include <chrono>
#include <random>
#include <string>
const int BUFF_SIZE = 64; // バッファのサイズ

using namespace std;
//乱数の初期化
/*
 * UDP Daytimeクライアント
 */

//任意の文字数の文字列を生成する関数
std::string random_string(int length) {
    std::random_device seed_gen;
    std::uniform_int_distribution<> dist(0x21,0x7e );//乱数を生成する関数の用意
    string str="" ;
    char c ;
  std::default_random_engine engine(seed_gen());
    for (int k = 0; k < length; k++){
        c= dist(engine);
        str +=c;
    }
    
    return str;
}




//!時間取得
double calcTime()
{
    struct::timespec getTime;
    clock_gettime(CLOCK_MONOTONIC, &getTime);
    return (getTime.tv_sec + getTime.tv_nsec*1e-9) *1000;
}






int main(int argc, char* argv[])
{
    
    using namespace std;
    cout << "upd time client v1.0.0" << endl; // ソースコードへの変更を行ったら数値を変える．
    string serv_ip = "127.0.0.1"; // ループバックアドレス
    in_port_t port_num = 5000; // ポート番号
    int n = 0; // 戻り値の保存用
    char buff[BUFF_SIZE]; // 送受信用バッファ

    if (argc == 2) {
        serv_ip = argv[1];
    }
    // パラメータの初期化
    struct sockaddr_in serv_addr; // アドレス構造体
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(serv_ip.c_str());
    serv_addr.sin_port = htons(port_num);

    // ソケットの作成．UDPを用いるため第２引数にDatagram，第３引数にUDPを指定する．
    int socketd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socketd < 0) {
        cout << "Failed to create a client socket.\n";
        return -1;
    }
    double start , end ;
    int trytime = 1000;
    double totaltime= 0;

    while (1)    {
    cout << "文字数を入力しよう: "<< endl;
    //ここを変えることによって送信内容を変えことができる。
//ここでクエリを送信する。　この時に好きな値を送信することができる。
        string msg  ;
        //cinで入力をすると長い文字列が入力できないから乱数を用いて入力を行う。
        //長い文字列を用いるのは通信時間を求めるためである。
        int num = 0;
        std::cin >> num;//ここはコメントアウトしておく
        msg = random_string(num);//ここでnum文字の文字列を生成している。
        //エコーバックにどれくらい実行時間がかかるかを計測する。
        for (int m = 0 ; m < trytime ;m++){
                   msg = "";
                    msg = random_string(num);//ここでnum文字の文字列を生成している。
        //std::cout << "送ったメッセージは msg: " << msg << std::endl;

        //送信開始
     start = calcTime();

//ここで送信を行う。nは送信した文字数を返す。nが-1の時はエラーが発生している。
    n = sendto(socketd, msg.c_str(), msg.size(), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));//msg.c_str()は文字列をchar型に変換する。
    if (n < 0) {//エラーが発生した時の処理
        cout << "failed to receive a message.\n";
        return -1;
    }
    // サーバから現在時刻を文字列として受信．
    n = recvfrom(socketd, buff, sizeof(buff)-1, 0, NULL, NULL); // 終端文字列を入れるために，sizeof(buff)-1 として，文字列一つ分必ず余裕を持たせてデータを受信する．buff をこのまま文字列として使わない場合は全記憶を受信に使う．
    if (n < 0) {//エラーが発生した時の処理
        cout << "Failed to receive a message.\n";
        return -1;
    }
    buff[n] = 0; // 終端文字列を追加．送信者が終端文字列を入れてデータを送ってきているとは限らない．
    cout << "Time: " << buff <<", " << htons(serv_addr.sin_port)<< "\n";
    
     end = calcTime();
//実行時間を表す変数の宣言
    double taketime = end - start ; 
    cout << "RTT time: " << taketime  << "ms" << endl;
    totaltime += taketime ;
        }
    }
    double averagetime = totaltime / trytime;
    cout << "Average RTT time: " << averagetime << "ms" << endl;
    sleep(1);//一秒間の間隔をあけるための関数。

    // ソケットを閉じる
    close(socketd);

    }
    
    
