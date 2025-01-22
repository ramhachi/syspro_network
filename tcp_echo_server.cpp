#include <arpa/inet.h>
#include <iostream>
#include <unistd.h> // https://linux.die.net/man/2/read
#include <string>

const int BUFF_SIZE = 64; // バッファのサイズ

int main(int argc, char *argv[])
{
    using namespace std;
    cout << "TCP echo server v1.0.0" << endl;

    int port_num = 5000; // ポート番号

    struct sockaddr_in serv_addr, clnt_addr; // ソケットアドレス
    int serv_socket, clnt_socket;            // ソケット記述子
    socklen_t addr_len;                      // アドレス長
    int n;                                   // 戻り値の保存用

    char buff[BUFF_SIZE]; // 送信用バッファ

    // パラメータの初期化
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port_num);
    addr_len = sizeof(clnt_addr);

    // 接続要求受付用のソケットを作成
    serv_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serv_socket < 0)
    {
        cout << "Failed to create a socket.\n";
        return -1;
    }
    // バインド（ソケットとポートの結合）
    if (bind(serv_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        cout << "Failed to bind a socket to the system.\n";
        return -1;
    }
    // ソケットをコネクション受け入れ可能な状態にする
    if (listen(serv_socket, 5) < 0)
    {
        cout << "Failed to listen to a socket.\n";
        return -1;
    }

    // クライアントから接続要求があれば、順次対応
    while (true)
    {
        cout << "Waiting for a client..." << endl;
        clnt_socket = accept(serv_socket, (struct sockaddr *)&clnt_addr, &addr_len);
        if (clnt_socket < 0)
        {
            cout << "Failed to accept a client.\n";
            continue;
        }

        // クライアントのIPアドレスとポート番号を表示
        cout << "Accepted a connection from [" << inet_ntoa(clnt_addr.sin_addr) << "," << htons(clnt_addr.sin_port) << "]" << endl;

        // クライアントからのメッセージを受信
        n = read(clnt_socket, buff, sizeof(buff) - 1);
        if (n <= 0)
        {
            cout << "Failed to read from a socket.\n";
            close(clnt_socket);
            continue;
        }
        buff[n] = '\0'; // 終端文字を追加

        cout << "Received message: " << buff << endl;

        // クライアントにメッセージをエコーバック
        n = write(clnt_socket, buff, n);
        if (n < 0)
        {
            cout << "Failed to write to a socket.\n";
            close(clnt_socket);
            continue;
        }

        // クライアントとの通信は終了したので、ソケットを閉じる
        close(clnt_socket);
    }

    // 受付用のソケットを閉じる
    close(serv_socket);
    return 0;
}
