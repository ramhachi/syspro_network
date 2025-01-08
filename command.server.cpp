//
// 東京都立大学 情報科学科
// 助教・柴田祐樹
// ２０１９年１０月　初版
// ２０２０年１０月　改訂
//
#include <arpa/inet.h>
#include <iostream>
#include <time.h>
#include <unistd.h> // https://linux.die.net/man/2/read

const int BUFF_SIZE = 64; // 一時記憶の長さ

int main(int argc, char *argv[])
{


    //セットアップするための部分（マイコンとかではsetup関数として纏められている部分）//
    using namespace std;
    cout << "command server v1.0.0" << endl;

    int port_num = 5000;

    struct sockaddr_in serv_addr, clnt_addr; // ソケットアドレス
    int serv_socket, clnt_socket;            // ソケット記述子
    socklen_t addr_len;                      // アドレス長

    time_t now;             // 時間
    char buff[BUFF_SIZE]; // 受信用一時記憶（６４バイト）

    // パラメータの初期化
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port_num);
    addr_len = sizeof(clnt_addr);

    serv_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//ソケットを作成

    if (serv_socket < 0)//ソケットが作成できない場合
    {
        cout << "Failed to create a socket.\n";
        return -1;
    }
    if (bind(serv_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)//ソケットをシステムにバインド
    {
        cout << "Failed to bind a socket to the system.\n";
        return -1;
    }
    if (listen(serv_socket, 5) < 0)
    {
        cout << "Filaed to listen to a socket.\n";
        return -1;
    }



    //メインループ（マイコンではloop関数として纏められている部分）//
    while (true)
    {
        cout << "Waiting for a client..." << endl;
        clnt_socket = accept(serv_socket, (struct sockaddr *)&clnt_addr, &addr_len);//クライアントからの接続を待つ
        // acceptはクライアントからの接続があるまで待機する実装がされている

        cout << "Accepted a connection from [" << inet_ntoa(clnt_addr.sin_addr) << "," << htons(clnt_addr.sin_port) << "]" << endl;//クライアントのIPアドレスとポート番号を表示
        string operation, operand;
        string os;
        int state = 0;//クライアントとの通信状態を表す．
        bool escape = false;
        double reg=0;
        while (state != 99)//クライアントからの接続が切れるまで繰り返す
        {
            time(&now);//現在時刻を取得
            cout << "state: " << state << ", waiting for messages..\n"; // 現在の状態を確認する
            
            //ここでクライアントからのメッセージを受信する．変数buffに受信した文字列が格納される．
            int n = read(clnt_socket, buff, BUFF_SIZE - 1);
            
            
            if (n <= 0) // 0は相手が接続を遮断したこと，負の値は接続に不具合が生じたことを意味する．正の値は受信に成功したことを意味する．
            {
                close(clnt_socket);//クライアントとの接続を切断
                break;
            }
            buff[n] = 0;
            cout << "got " << n << " byte, " << buff  << "\n";
            for (int i = 0; i < n; ++i) // 受け取った文字列を一つずつ（1Byte毎）に検査する．
            {
                if (buff[i] == '\"')//ダブルクォーテーションが来たら，エスケープ状態をトグルする．
                {
                    escape = !escape;//エスケープ状態をトグルする.この実装のおかげで、ダブルクォーテーションの中の文章の空白を無視することなく、文字列として扱うことができる。
                }

                else if (escape)
                {
                    os += buff[i]; // ""で囲まれた中なので，空白が来たとしても文字列として加算する．
                }

                else if (buff[i] == ' ' || buff[i] == '\n') // 空白が来たところで文字列を区切る．改行コードも空白と定義．タブを含めても良い．
                {
                    if (state == 0) // 命令を受け取る状態
                    {
                        operation = os;//命令を格納
                        if(operation == "get reg"){//レジスタの値を返す
                            string ret = to_string(reg);//レジスタの値を文字列に変換
                            write(clnt_socket, ret.c_str(), ret.size());//クライアントに送信
                            state = 0;//命令待機状態へ戻る
                            cout << "1\n";
                        }
                        else if(operation == "end process"){
                            state = 99; // 計算を終了
                            cout << "2\n";
                        }
                        else{
                            state = 1;  // 次のオペランドを確認
                        }
                    }
                    else if (state == 1)
                    {
                        operand = os;//オペランドを格納

                        double a = stod(operand);//オペランドをdouble型に変換
                        if(operation == "set reg"){
                            reg = a;
                            cout << "3\n";
                        }
                        else if(operation == "add"){
                            reg += a;
                            cout << "4\n";
                        }
                        else if(operation == "subtract"){
                            reg -= a;
                            cout << "5\n";
                        }
                        else if(operation == "multiply"){
                            reg *= a;
                            cout << "6\n";
                        }else if(operation == "divide"){
                            reg /= a;
                            cout << "7\n";
                        }
                        state = 0; // 命令待機状態へ戻る．
                    }
                    os = "";
                }
                else
                {
                    os += buff[i]; // 空白が来るまで文字列へ文字を格納
                }
            }
        }
        close(clnt_socket);
    }
    close(serv_socket);
    return 0;
}
