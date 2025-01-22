#include <arpa/inet.h>
#include <iostream>
#include <unistd.h> // https://linux.die.net/man/2/read
#include <string>
#include <chrono>
#include <ctime>
#include <random>

const int BUFF_SIZE = 64; // バッファのサイズ

// 任意の文字数の文字列を生成する関数
std::string random_string(int length) {
    std::random_device seed_gen;
    std::uniform_int_distribution<> dist(0x21, 0x7d); // 0x7e (tilde) is excluded to avoid null characters
    std::string str = "";
    char c;
    std::default_random_engine engine(seed_gen());
    for (int k = 0; k < length; k++) {
        c = dist(engine);
        str += c;
    }
    return str;
}

// 時間取得
double calcTime() {
    struct timespec getTime;
    clock_gettime(CLOCK_MONOTONIC, &getTime);
    return (getTime.tv_sec + getTime.tv_nsec * 1e-9) * 1000;
}

int main(int argc, char* argv[]) {
    using namespace std;
    cout << "TCP echo client v1.0.0" << endl;

    string serv_ip = "127.0.0.1"; // サーバのIPアドレス
    in_port_t serv_port = 5000;   // サーバのポート番号

    if (argc > 1) {
        serv_ip = argv[1];
    }

    char buff[BUFF_SIZE]; // 受信用バッファ
    int n = 0;            // 戻り値の保存用に使う変数

    // ソケット作成，入力はIP，ストリーム型，TCPを指定
    int socketd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socketd < 0) {
        cout << "Failed to create a socket\n";
        return -1;
    }
    // サーバのアドレス等を初期化
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(serv_ip.c_str());
    serv_addr.sin_port = htons(serv_port);

    // サーバに接続する
    n = connect(socketd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (n < 0) {
        cout << "Failed to connect to the server\n";
        return -1;
    }

    string msg;
    while (true) {
        cin >> msg;
//もし、msgが改行なら無視する
        if (msg == "\n") {
            continue;
        }
        if (msg == "END") {
            break;
        }
        if (msg == "RANDOM") {
            cout << "ランダムな文字列の長さをを入力してください\n";
            int length;
            cin >> length;
            msg = random_string(length);
            cout << "ランダムな文字列は" << msg << "です" << endl;
        }
        string msgcopy = msg;

        // 送信する文字列に終端文字を追加
        msg += '\0';
        double starttime = calcTime();
        n = write(socketd, msg.c_str(), msg.size()); // 文字列の送信
        if (n < 0) {
            cout << "failed to write to a socket\n";
            return -1;
        }

        // read(.)により，データを受信する
        n = read(socketd, buff, sizeof(buff) - 1);
        if (n < 0) {
            // readの戻り値が負の場合，通信に不具合が生じたことを意味する
            cout << "failed to read from a socket\n";
            return -1;
        }

        // readの戻り値が 0 の場合，相手が接続を遮断したことを意味する
        if (n == 0) {
            cout << "Connection closed by the server\n";
            break;
        }

        buff[n] = '\0'; // 終端文字を追加

        if (msgcopy == buff) {
            cout << "OK。エコーバックされた文字列は正しいです" << endl;
        } else {
            cout << "NGエコーバックされた文字列は異なります" << endl;
            // buffの表示
            cout << "Received a message: " << buff << endl;
            // 何文字目でエラーが起きたかを表示
            for (int i = 0; i < msgcopy.length(); i++) {
                if (msgcopy[i] != buff[i]) {
                    cout << "Error occured at " << i << "th character" << endl;
                    // エラーが起きた文字を表示
                    cout << "Error character: " << buff[i] << endl;
                    // エラーが起きた文字のASCIIコードを表示
                    cout << "Error character's ASCII code: " << (int)buff[i] << endl;
                    // 正しい文字を表示
                    cout << "Correct character: " << msgcopy[i] << endl;
                    // 正しい文字のASCIIコードを表示
                    cout << "Correct character's ASCII code: " << (int)msgcopy[i] << endl;
                    break;
                }
            }
        }
        double endtime = calcTime();
        cout << "通信時間は" << endtime - starttime << "msです" << endl;
        // buffの初期化
        memset(buff, 0, sizeof(buff));
    }
    // ソケットを閉じる
    close(socketd);
    return 0;
}
