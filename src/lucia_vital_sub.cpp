/*
 * バイタル信号受信プログラム
 * Version 1.0  2022.02.01  K.Ohno
 *
 * このプログラムはシリアルポートをopenして、バイタル通信基盤からの信号を16進数表示するプログラムです。
 * Ubuntu 20.04で動作検証をしています。
 *
 * test-machine: nuc2,nuc28
 */
#include "include/lucia_vital_sub/ARM_CONTROLL.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <typeinfo>
#include <vector>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <sys/wait.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

// yarp related /////////////////////
//#include <yarp/os/all.h>
//using namespace yarp::os;
using namespace cv;
using namespace std;

#define DEV_NAME    "/dev/ttyUSB0"        // デバイスファイル名
#define BAUD_RATE    2000000              // RS232C通信ボーレート
#define BUFF_SIZE    1000                 // データ長さ（バイタルの場合８バイト）の２〜３倍

std::vector<double> val_Pulse;
std::vector<double> val_Spo2;
std::vector<double> val_BloPre_max;
std::vector<double> val_BloPre_min;

double avg_Pulse;
double avg_Spo2;
double avg_BloPre_max;
double avg_BloPre_min;

int vital, pressure, stage, messure, angle= 0;
ARM_CONTROLL ArmCont;

string input;

Point pos(-15,-15);//初期位置
Mat img(1100, 1100, CV_8UC3);//y,x
//img = Scalar(122, 122, 122); //カラー（RGB）

bool areAllNonZero(const std::vector<int>& vec) {
    for (int value : vec) {
      if (value <= 0)
	{
	std::cout<< "1find" << std::endl;
	return true;
	}

    }
    std::cout<< "0find" << std::endl;
    return false;

}

void DISPLAY(int stage)
	{
		std::string text;
		cv::Mat img = cv::Mat::zeros(1100, 1100, CV_8UC3); // 黒背景

		// 必要に応じて再塗りつぶし（初期化）
		img.setTo(cv::Scalar(0, 0, 0)); // 背景を黒に設定

		// フォントの種類
		int fontFace = cv::FONT_HERSHEY_SIMPLEX;

		// フォントスケール
		double fontScale =1.0;

		// 色（BGR形式）
		cv::Scalar color(0, 255, 0);

		cv::Point textOrg(500, 500);

		// 太さ
		int thickness = 1;

		// アンチエイリアスを有効にするフラグ
		int lineType = cv::LINE_AA;

		 // 描画する文字列
		if(stage == 0)
		{
		text = "Waiting...";
		cv::putText(img, text, textOrg, fontFace, fontScale, color, thickness, lineType);
		}
		else if(stage == 1)
		{
		text = "Hello！";
		cv::putText(img, text, textOrg, fontFace, fontScale, color, thickness, lineType);
		}
		else if(stage == 2)
		{
		text = "Measuring...";
		cv::putText(img, text, textOrg, fontFace, fontScale, color, thickness, lineType);
		}
		else if(stage == 11)
		{
		cv::Point text1Org(50, 450);
		cv::Point text2Org(50, 550);
		std::string text1 = "The measurement seems to have an issue ";
		std::string text2 = "Do you want to continue? (y/n)";


		cv::putText(img, text1, text1Org, fontFace, fontScale, color, thickness, lineType);
		cv::putText(img, text2, text2Org, fontFace, fontScale, color, thickness, lineType);

		}
		else if(stage == 12)
		{
		cv::Point text1Org(50, 450);
		cv::Point text2Org(50, 550);
		std::string text1 = "Please enter 'y' or 'n'.";
		std::string text2 = "Do you want to continue? (y/n)";

		cv::putText(img, text1, text1Org, fontFace, fontScale, color, thickness, lineType);
		cv::putText(img, text2, text2Org, fontFace, fontScale, color, thickness, lineType);

		}




		// ウィンドウに画像を表示
		cv::imshow("img", img);
		int key = cv::waitKey(50);
	}

void display_vital(float Pulse, float spo2, float bp_max, float bp_min)
	{



	cv::Mat img = cv::Mat::zeros(1100, 1100, CV_8UC3); // 黒背景

    // 必要に応じて再塗りつぶし（初期化）
    img.setTo(cv::Scalar(0, 0, 0)); // 背景を黒に設定

	if (img.empty()) {
   	 std::cerr << "Error: img initialization failed." << std::endl;
    	return;
	}

	cv::imshow("img", img);


	// std::ostringstream を使って文字列を作成
	std::ostringstream oss1, oss2, oss3;

	// 小数点以下1桁にフォーマット
	oss1.precision(1);
	oss1 << std::fixed << "Pulse: " << Pulse;

	oss2.precision(1);
	oss2 << std::fixed << "SpO2: " << spo2;

	oss3.precision(1);
	oss3 << std::fixed << "BloPre(max/min): " << bp_max<< "/" << bp_min;

	    // 文字列を取得
	std::string PULSE = oss1.str();
	std::string SPO2 = oss2.str();
	std::string BP = oss3.str();
	std::string text = "Your vital signs are...";




		// フォントの種類
		int fontFace = cv::FONT_HERSHEY_SIMPLEX;

		// フォントスケール
		double fontScale =2.0;

		// 色（BGR形式）
		cv::Scalar color(0, 255, 0);


		// 太さ
		int thickness = 2;

		// アンチエイリアスを有効にするフラグ
		int lineType = cv::LINE_AA;

	cv::Point textOrg(50, 100);
	cv::putText(img, text, textOrg, fontFace, fontScale, color, thickness, lineType);

	cv::Point textOrg1(50, 400);
	cv::putText(img, PULSE, textOrg1, fontFace, fontScale, color, thickness, lineType);

	cv::Point textOrg2(50, 600);
	cv::putText(img, SPO2, textOrg2, fontFace, fontScale, color, thickness, lineType);

	cv::Point textOrg3(50, 800);
	cv::putText(img, BP, textOrg3, fontFace, fontScale, color, thickness, lineType);



	// ウィンドウに画像を表示
	cv::imshow("img", img);
	int key = cv::waitKey(50);

	}

// シリアルポートの初期化
void serial_init(int fd)
{
    struct termios tio;

    memset(&tio,0,sizeof(tio));
    tio.c_cflag = CS8 | CLOCAL | CREAD;
    tio.c_cc[VTIME] = 100;

    // ボーレートの設定	１
    cfsetispeed(&tio,BAUD_RATE);
    cfsetospeed(&tio,BAUD_RATE);

    // デバイスに設定を行う
    tcsetattr(fd,TCSANOW,&tio);
}

int main(int argc,char *argv[]){
	angle = 0;
	ArmCont.arm_controll(angle);
	usleep(2*1000000);



	imshow("img",img);
	int key = waitKey(50);
	DISPLAY(stage);

	std::cout<<"ready?"<<std::endl;
	std::cin>>input;



	angle = 130;
	ArmCont.arm_controll(angle);



	imshow("img",img);



	usleep(2*1000000);


	int fd;
	// デバイスファイル（シリアルポート）オープン
	fd = open(DEV_NAME,O_RDWR);

	if(fd < 0){
		// デバイスの open() に失敗したら
		perror(argv[1]);
		exit(1);
	}

	cout << "fd:" << fd << endl;

	// シリアルポートの初期化
	serial_init(fd);

	// メインの無限ループ


	while(1){

		unsigned char buffer[BUFF_SIZE];

		// ファイルディスクリプターから最大BUFF_SIZEバイトをbufferへ読み込み、返り値は読み込んだバイト数
		int len = read(fd, buffer, BUFF_SIZE);
		cout << "len:" << len << endl;
		if(len == 0){
		    // read()が0を返したら、end of file
		    // 通常は正常終了するのだが今回は無限ループ
		    continue;
		}

		if(len < 0){
		    printf("%s: ERROR\n", argv[0]);
		    // argv[0]にはプログラム名
		    perror("");
		    exit(2);
		}

		vector<int> vec = {};

		// データ切り出し（パケットヘッダ0xAAからエンドコード0x55まで）
		//C++で増減するデータをスマートに扱うならvector(固定長のデータはarray)

		for (int i = 0; i < len; i++) {
		    if (buffer[i] == 0xAA) {  // パケットの開始を検出
			vec.clear();  // 新しいパケットの開始時にクリア
			vec.push_back(buffer[i]);
			i++;  // 0xAAの次のバイトから開始
			while (i < len && buffer[i] != 0x55) {  // 0x55が見つかるまで、またはバッファの終わりまで
			    vec.push_back(buffer[i]);
			    i++;
			}
			if (i < len) {  // 0x55が見つかった場合
			    // ここでvecを使用する

			    for(int j=0; j <= vec.size(); j++)
			    {
			    	vec[j] = buffer[j];
			    }
			 }
			    // データ処理ロジックをここに記述
			  //cout << "Received valid packet: ";
			    for (int v : vec) {
			      //  cout << v << " ";
			    }

			    if (i < len && vec.size() >= 12) {  // パケット終了検出とデータ長の確認
                 		//cout << "Received valid packet: ";
		            for (int v : vec) {
		                //cout << v << " ";
		            }
				  //  cout << endl;
			}
			// パケットが不完全な場合は、何もしない
		    }
		}






		//基盤ID=10,11,12
		if(vec.size() > 2 && vec[2]==10 || vec[2]==11 || vec[2]==12){

			double spo2 = (double)(vec[6]*256+vec[5])*0.1;
			//double bp = (double)(vec[8]*256+vec[7])*0.1;

			//バイタル
			if(vec[1]==197){
				cout << "[Id=" << vec[2] << flush;
				cout << "] Pulse " << vec[4] << flush;
				cout << "| SpO2 " << spo2 << flush;
				cout << "| BloPre(max/min) " << vec[7] <<"/" <<vec[8] << endl;

				if(vec[4] <= 150.0 and vec[4] >= 30.0){ //脈拍異常値判定
					val_Pulse.push_back(vec[4]);

					if(val_Pulse.size() > 15.0){
						val_Pulse.erase(val_Pulse.begin());
					}
				}
				if(spo2 >= 80.0  and spo2 < 100){ //Spo2異常値判定
					val_Spo2.push_back(spo2);

					if(val_Spo2.size() > 15.0){
						val_Spo2.erase(val_Spo2.begin());
					}
				}
				if( vec[7] <= 150.0 and vec[7] >= 70){ //最大血圧異常値判定
					val_BloPre_max.push_back(vec[7]);

					if(val_BloPre_max.size() > 15.0){
						val_BloPre_max.erase(val_BloPre_max.begin());
					}
				}
				if(vec[8] >= 50.0  and vec[8] >= 0){ //最小血圧異常値判定
					val_BloPre_min.push_back(vec[8]);

					if(val_BloPre_min.size() > 15.0){
						val_BloPre_min.erase(val_BloPre_min.begin());
					}
				}

				if(areAllNonZero(vec)){
					vital = 1;
				}
				else{
					vital = 0;
				}

				if(val_Pulse.size() == 15 and val_Spo2.size() == 15 and val_BloPre_max.size() == 15 and val_BloPre_min.size() == 15){
					double sum_Pulse;
					double sum_Spo2;
					double sum_BloPre_max;
					double sum_BloPre_min;


					for(double value : val_Pulse){
						sum_Pulse += value;
					}
					for(double value : val_Spo2){
						sum_Spo2  += value;
					}
					for(double value : val_BloPre_max){
						sum_BloPre_max += value;
					}
					for(double value : val_BloPre_min){
						sum_BloPre_min  += value;
					}

					avg_Pulse = sum_Pulse/val_Pulse.size();
					avg_Spo2  = sum_Spo2/val_Spo2.size();
					avg_BloPre_max = sum_BloPre_max/val_BloPre_max.size();
					avg_BloPre_min = sum_BloPre_min/val_BloPre_min.size();

					cout << "脈拍:" << avg_Pulse << flush;
					cout << "  |血中酸素飽和度:" << avg_Spo2 << flush;
					cout << "  |血圧（収縮期/膨張期): " << avg_BloPre_max <<"/" << avg_BloPre_min << endl;

					display_vital(avg_Pulse, avg_Spo2, avg_BloPre_max, avg_BloPre_min);
					std::cin>>input;
					break;
				}
				/*
				messure++;
				if(messure >= 10){

					stage = 11;
					DISPLAY(stage);

					while(1)
					{
						 cout << "Enter 'y' or 'n': ";
						std::cin>>input;
						if(input != "y" && input != "n"){
						stage = 12;
						DISPLAY(stage);
						}
					}


					if(input == "y") {
					messure = 0;
					}
					else if(input == "n") {
					break;
					}
				}
				*/

		}

			//圧力

			else if(vec[1]==200){

				int key = waitKey(50);
				int tier_x,tier_y = 0;

				cout << "[Id=" << vec[2] << flush;
				cout << "] " << vec[4] << flush;
				cout << ", " << vec[5] << flush;
				cout << ", " << vec[6] << flush;
				cout << ", " << vec[7] << flush;
				cout << ", " << vec[8] << flush;
				cout << ", " << vec[9] << flush;
				cout << ", " << vec[10] << flush;
				cout << ", " << vec[11] << endl;

				for(int i = 4 ; i == 11; i++)
				{
					if(vec[i] <= 50){
					vec[i] = 0;
					std::cout << vec[i] << "= 0"<<std::endl;
					}
				}
			if(vec[2] == 10){
				tier_x = 5;
				tier_y = 1;
			}
			else if(vec[2] == 11){
				tier_x = 2;
				tier_y = 2;
			}
			else if(vec[2] == 12){
				tier_x = 0;
				tier_y = 0;
			}

			if(areAllNonZero(vec)){
				pressure = 1;
			}
			else{
				pressure = 0;
			}



			}
			///////////////////////////////////////////////////////////////////////////////////////
			else{
				cout << "[ID=" << vec[2] << flush;
				cout << "] ERROR " << vec[4] << endl;
			}
		}

		//基盤ID=9
		else if(vec[2]==9){

			double pt1 = (double)(vec[5]*256+vec[4]);
			double pt2 = (double)(vec[7]*256+vec[6]);
			double pt3 = (double)(vec[9]*256+vec[8]);
			double pt4 = (double)(vec[11]*256+vec[10]);

			//ポテンショ
			if(vec[1]==200){
				cout << "[Id=" << vec[2] << flush;
				cout << "] " << pt1 << flush;
				cout << ", " << pt2 << flush;
				cout << ", " << pt3 << flush;
				cout << ", " << pt4 << endl;
			}
			//電磁弁
			else if(vec[1]==194){
				cout << "[Id=" << vec[2] << flush;
				cout << "] " << vec[4] << flush;
				cout << ", " << vec[5] << endl;
			}
			else{
				cout << "[ID=" << vec[2] << flush;
				cout << "] ERROR " << vec[4] << endl;
			}
		}
		else{
			cout << "[INVALID ID=" << vec[2] << flush;
			cout << "] ERROR " << vec[4] << endl;
		}

		if(vital == 0 and pressure == 0){
			stage = 0;
		}
		else if(vital == 1 and pressure == 1){
			stage = 2;
		}
		else if(vital == 2){
			stage = 3;
		}

		std::cout<< "vital:"<<vital << " pressure:"<<pressure<<std::endl;
		DISPLAY(stage);


	}
	angle = 0;
	ArmCont.arm_controll(angle);
}
