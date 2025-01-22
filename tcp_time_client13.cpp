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
#include <unistd.h> // https://linux.die.net/man/2/read
#include <string>
#include <ctime>
#include <bits/stdc++.h>
const int BUFF_SIZE = 1024; // バッファのサイズを1024に増やす


//任意の文字数の文字列を生成する関数
std::string random_string(int length) {
    std::random_device seed_gen;
    std::uniform_int_distribution<> dist(0x21, 0x7d); // 0x7e (tilde) is excluded to avoid null characters
   std::string str="" ;
    char c ;
  std::default_random_engine engine(seed_gen());
    for (int k = 0; k < length; k++){
        c= dist(engine);
        str +=c;
    }
    
    return str;
}


//!時間取得
double calcTime()//これはこのサイトのものを流用した。https://qiita.com/yagiyuki/items/7d9b2512469b29ca88b9
{
    struct::timespec getTime;
    clock_gettime(CLOCK_MONOTONIC, &getTime);
    return (getTime.tv_sec + getTime.tv_nsec*1e-9) *1000;
}



int main(int argc, char* argv[])
{
    double starttime = 0;
    double endtime = 0;
    using namespace std;



    string msg;
//ここから、サーバに接続する処理ーーーーーーーー
    while(1){
        //先にサーバとの接続を確立する。
    
    cout << "TCP time client v1.0.2" << endl; // ソースコードへの変更を行ったら数値を変える．

    // サーバのアドレスとポート番号
    // 127.0.0.1は，ループバックアドレス
    // 他のPCと通信する場合は，当該PCのIPアドレスに変更する．
    string serv_ip = "127.0.0.1";
    in_port_t serv_port = 5000;
    
    if(argc > 1)
    {
	    serv_ip = argv[1];
    }


    char buff[BUFF_SIZE];// 受信用バッファ
    int n = 0; // 戻り値の保存用に使う変数．

    // ソケット作成，入力はIP，ストリーム型，TCPを指定．
    int socketd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socketd < 0) {
        cout << "Failed to createa socket\n";
        return -1;
    }
    // サーバのアドレス等を初期化．
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(serv_ip.c_str());
    serv_addr.sin_port = htons(serv_port);

    // サーバに接続する．
    n = connect(socketd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (n < 0) {
        cout << "Failed to connect to the server\n";
        return -1;
    }

       // cin >> msg;
       msg = "RANDOM";//ランダムな文字列しか使わないのでここは固定してしまう

        if(msg == "END")
        {
            break;
        }
        if (msg == "RANDOM")
        {
            cout << "ランダムな文字列の長さをを入力してください\n";
            int length;
            std::cin >> length;
            msg = random_string(length);
            cout << "ランダムな文字列は" << msg << "です" << endl;
        }
        std::string msgcopy = msg;

        // 送信する文字列に終端文字を追加
        msg += '\0';
        
        starttime = calcTime();
        n = write(socketd, msg.c_str(), msg.size()); // 文字列の送信．第二引数は記憶域．第３引数は送信するByte数．
        if (n < 0) {
            cout << "failed to write to a socket\n";
            return -1;
        }

        // read(.)により，データを受信する．
        n = read(socketd, buff, sizeof(buff)-1);
        if (n < 0) {
            // readの戻り値が負の場合，通信に不具合が生じたことを意味する．
            cout << "failed to read from a socket\n";
            return -1;
        }
        
        // readの戻り値が 0 の場合，相手が接続を遮断したことを意味する．
        if (n == 0) {
            cout << "Connection closed by the server\n";
            break;
        }
        buff[n] = '\0'; // 終端文字を追加
        endtime = calcTime();
        cout << "通信時間は" << endtime - starttime << "msです" << endl;

        std::string buff_str(buff);
        for (int i = 0; i < n; i++)
        {
            if (buff_str[i] == '\0')
            {
            buff_str.erase(i, 1);//バッファの終端文字を削除
            }
        }
        //buffstrの終端文字の個数を数える
        int count = 0;
        for (int i = 0; i < buff_str.length(); i++)
        {
            if (buff_str[i] == '\0')
            {
                count++;
            }
        }
        cout << "終端文字の数" << count << endl;
        
        if (msgcopy == buff_str)
        {
            cout << "OK。エコーバックされた文字列は正しいです" << endl;
        }
        else
        {
            cout << "NGエコーバックされた文字列は異なります" << endl;
            //buffの表示
            //cout << "Received a message: " << buff_str << endl;
            //何文字目でエラーが起きたかを表示
            for (int i = 0; i < msgcopy.length(); i++)
            {
                if (msgcopy[i] != buff_str[i])
                {
                    cout << "Error occured at " << i << "th character" << endl;
                    //エラーが起きた文字を表示
                    cout << "Error character: " << buff_str[i] << endl;
                    //エラーが起きた文字のASCIIコードを表示
                    cout << "Error character's ASCII code: " << (int)buff_str[i] << endl;
                    //正しい文字を表示
                    cout << "Correct character: " << msgcopy[i] << endl;
                    //正しい文字のASCIIコードを表示
                    cout << "Correct character's ASCII code: " << (int)msgcopy[i] << endl;
                    break;
                }
            }
        }

        
        //buffの初期化
        memset(buff, 0, sizeof(buff));
        //budd2の初期化
        //通信は終わったので、ソケットを閉じる。
        close(socketd);
    }
}
