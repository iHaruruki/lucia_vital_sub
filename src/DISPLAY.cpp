#include "DISPLAY.h"

#include <iostream>
#include <cmath>
#include <math.h>
#include <fstream>//
#include <unistd.h>
#include <time.h>
#include <stdio.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>


	void DISPLAY::display_vital(VitalData& VData)
	{

	// std::ostringstream を使って文字列を作成
	std::ostringstream oss1, oss2, oss3;

	// 小数点以下1桁にフォーマット
	oss1.precision(1);
	oss1 << std::fixed << "脈拍: " << VData.Pulse;

	oss2.precision(1);
	oss2 << std::fixed << "SpO2: " << VData.spo2;

	oss3.precision(1);
	oss3 << std::fixed << "血圧（収縮期/膨張期）: " << VData.bp_max<< "/" << VData.bp_min;

	    // 文字列を取得
	std::string PULSE = oss1.str();
	std::string SPO2 = oss2.str();
	std::string BP = oss3.str();


	int key = cv::waitKey(50);


	// フォントの種類
	int fontFace = cv::FONT_HERSHEY_SIMPLEX;

	// フォントスケール
	double fontScale = 1.0;

	// 色（BGR形式）
	cv::Scalar color(255, 255, 255);

	// 太さ
	int thickness = 2;

	// アンチエイリアスを有効にするフラグ
	int lineType = cv::LINE_AA;

	std::string text = "あなたのバイタルは...";
	cv::Point textOrg1(50, 750);
	cv::putText(img, PULSE, textOrg1, fontFace, fontScale, color, thickness, lineType);

	cv::Point textOrg2(50, 500);
	cv::putText(img, SPO2, textOrg2, fontFace, fontScale, color, thickness, lineType);

	cv::Point textOrg3(50, 1000);
	cv::putText(img, BP, textOrg3, fontFace, fontScale, color, thickness, lineType);

	cv::Point textOrg(50, 250);
	cv::putText(img, text, textOrg, fontFace, fontScale, color, thickness, lineType);

	// ウィンドウに画像を表示
	cv::imshow("img", img);

	// 任意のキー入力で終了
	cv::waitKey(0);

	}


	void DISPLAY::display(DisplayData& DData)
	{
		std::string text = "待機中...";

		// フォントの種類
		int fontFace = cv::FONT_HERSHEY_SIMPLEX;

		// フォントスケール
		double fontScale = 100.0;

		// 色（BGR形式）
		cv::Scalar color(0, 255, 0);

		cv::Point textOrg(500, 500);

		// 太さ
		int thickness = 2;

		// アンチエイリアスを有効にするフラグ
		int lineType = cv::LINE_AA;

		 // 描画する文字列
		if(DData.stage == 0)
		{
		std::string text = "待機中...";
		}
		else if(DData.stage == 1)
		{
		std::string text = "こんにちは！";
		}
		else if(DData.stage == 2)
		{
		std::string text = "計測中...";
		}

		cv::putText(img, text, textOrg, fontFace, fontScale, color, thickness, lineType);
		// ウィンドウに画像を表示
		cv::imshow("img", img);
	}
