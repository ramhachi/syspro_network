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
const int BUFF_SIZE = 64; // バッファのサイズ

using namespace std;
//乱数の初期化
/*
 * UDP Daytimeクライアント
 */

//任意の文字数の文字列を生成する関数
std::string random_string(int length) {
    auto randchar = []() -> char {//関数内で関数を定義している。　この関数はchar型の文字を返す。
        const char charset[] =
                "0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz"
                "!@#$%^&*()_+{}|:<>?~";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[rand() % max_index];//乱数を生成している。
    };
    std::string str(length, 0);
    std::generate_n(str.begin(), length, randchar);//generate_nは第一引数に指定された数だけ第三引数の関数を呼び出す。
    return str;
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
    while (1)    {
    cout << "Enter a message: "<< endl;
    //ここを変えることによって送信内容を変えことができる。
//ここでクエリを送信する。　この時に好きな値を送信することができる。
        string msg  ;
        //cinで入力をすると長い文字列が入力できないから乱数を用いて入力を行う。
        //長い文字列を用いるのは通信時間を求めるためである。
        //std::cin >> msg;//ここはコメントアウトしておく
        msg = random_string(10000);//ここで10000文字の文字列を生成している。
        //エコーバックにどれくらい実行時間がかかるかを計測する。
        std::cout << "送ったメッセージは msg: " << msg << std::endl;

        //送信開始
    start = chrono::system_clock::now();//ここで時間を計測する

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
    end = chrono::system_clock::now();
//実行時間を表す変数の宣言
double taketime = chrono::duration_cast<chrono::milliseconds>(end - start).count();//型変換を用いてミリ秒に変換している。
    cout << "RTT time: " << taketime  << "ms" << endl;
//一秒間の間隔をあける。
    sleep(1);//一秒間の間隔をあけるための関数。

    // ソケットを閉じる
    close(socketd);
}
    }
    
    
