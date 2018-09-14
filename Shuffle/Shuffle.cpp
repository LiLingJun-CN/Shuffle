/*
 * Copyright (c) 2017-
 *
 * 該文件僅限於wintokens遊戲驗牌，禁止其它壹切用途。
 *
 * 使用方法：
 * 1，BlackJackShuffle() 函數接收所有種子，壹次性運算出最終洗牌結果。
 * 2，工具函數，方便逐步驗證：
 *		GenClientSeedForShow() 將洗牌種子進行md5加密，與遊戲中聊天窗口相關顯示比對。
 *		GenFinallySeed() 使用種子列表生成最終洗牌種子。
 *		GenRandomListBySeed()	根據洗牌種子生成隨機數序列。
 *		Shuffle() 交換洗牌算法得出最終撲克牌序列。
 */

#include <string>
#include <vector>
#include <random>
#include "Crypto.h"
using namespace std;

static std::string sCardGraph[4] = { "方片", "梅花", "紅桃", "黑桃" };
static std::string sCardNumStr[14] = { "", "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K" };

int GetPokerValue(int n, int maxvalue)
{
	return (n % maxvalue) == 0 ? maxvalue : (n % maxvalue);
}

int GetPokerType(int n, int maxvalue)
{
	int type = (n % maxvalue) == 0 ? (n / maxvalue - 1) : (n / maxvalue);
	return type % 4;
}

//生成加密的客戶提供種子，用於客戶端顯示
std::string GenClientSeedForShow(std::string clientSeed)
{
	return BotanCrypto::md5(clientSeed);
}

// 使用客戶端玩家提供的種子及全局唯壹洗牌ID
std::string GenFinallySeed(
	std::string playerSeed1,
	std::string playerSeed2,
	std::string playerSeed3,
	std::string playerSeed4,
	std::string playerSeed5,
	std::string playerSeed6,
	std::string playerSeed7,
	std::string playerSeed8,
	std::string shuffleID)
{
	printf("Player Seeds：%s %s %s %s %s %s %s, %s\n",
		playerSeed1.c_str(), playerSeed2.c_str(), playerSeed3.c_str(), playerSeed4.c_str(),
		playerSeed5.c_str(), playerSeed6.c_str(), playerSeed7.c_str(), playerSeed8.c_str());

	printf("Shuffe ID：%s\n", shuffleID.c_str());

	std::string finallySeed = playerSeed1 + playerSeed2 + playerSeed3 + playerSeed4
		+ playerSeed5 + playerSeed6 + playerSeed7 + playerSeed8 + shuffleID;

	printf("Finally Seed：%s\n\n", finallySeed.c_str());
	return finallySeed;
}

// 使用最終洗牌種子進行洗牌
std::vector<uint64_t> GenRandomListBySeed(std::string seed, int needNumCnt)
{
	std::vector<uint64_t> randNumList;
	if (needNumCnt <= 0)
		return randNumList;

	std::seed_seq tmpSeed(seed.begin(), seed.end());
	std::mt19937_64 gen(tmpSeed);
	for (int i = 0; i < needNumCnt; ++i)
		randNumList.push_back(gen());
	return std::move(randNumList);
}

/*
 *  cardCnt      該玩法有多少張牌
 */
std::vector<int> Shuffle(const std::vector<uint64_t>& randomList, int cardCnt)
{
	std::vector<int> cardList;
	if (cardCnt <= 0)
		return cardList;

	for (int i = 0; i < cardCnt; ++i)
		cardList.push_back(i + 1);

	for (int i = 0; i < (int)randomList.size(); ++i)
		std::swap(cardList[i%cardCnt], cardList[randomList[i] % cardCnt]);
	return std::move(cardList);
}

/*
 *  cardMaxValue 該玩法最大牌面值
 */
void PokerPrint(const std::vector<int>& cardList, int actPokerMaxValue)
{
	int cnt = 0;
	printf("Visual card sequence：\n");
	const int cardSize = (int)cardList.size();
	for (int i = 0; i < cardSize; ++i)
	{
		if (0 != i && 0 == i % 52)
			putchar('\n');

		++cnt;
		printf("%s%s ",
			sCardGraph[GetPokerType(cardList[i], actPokerMaxValue)].c_str(),
			sCardNumStr[GetPokerValue(cardList[i], actPokerMaxValue)].c_str());
	}

	putchar('\n');
}

/*
 *  finallySeed  最終種子
 *  cardCnt      該玩法有多少張牌
 *  cardMaxValue 該玩法最大牌面值
 */
void PokerPrintShuffle(const std::string& finallySeed, int cardCnt, int cardMaxValue)
{
	if (cardCnt <= 0 || cardMaxValue <= 0)
	{
		printf("%s : %d : Parameter Error\n", __FUNCTION__, __LINE__);
		return;
	}

	std::vector<uint64_t> randomList = GenRandomListBySeed(finallySeed, cardCnt);
	printf("RandomList：\n");
	for (uint64_t num : randomList)
		printf("%d ", (int)(num % cardCnt));
	printf("\n\n");

	std::vector<int> cardList = Shuffle(randomList, cardCnt);
	printf("cardList：\n");
	for (int card : cardList)
		printf("%d ", card);

	printf("\n\n");

	PokerPrint(cardList, cardMaxValue);
}

// 股子沒有涉及到牌面值轉換
void DiceShuff(const std::string& finallySeed, int diceCnt)
{
	if (diceCnt <= 0)
	{
		printf("%s : %d : Parameter Error\n", __FUNCTION__, __LINE__);
		return;
	}

	std::vector<uint64_t> randomList = GenRandomListBySeed(finallySeed, diceCnt);
	printf("RandomList：\n");
	for (uint64_t num : randomList)
		printf("%d ", (int)(num % diceCnt));
	printf("\n\n");

	std::vector<int> diceList = Shuffle(randomList, diceCnt);
	printf("DiceList：\n");
	for (int dice : diceList)
		printf("%d ", dice);
	printf("\n\n");

	printf("Finally DiceList：\n");
	for (int dice : diceList)
		printf("%d ", (dice % 6) + 1);

	putchar('\n');
}

int main()
{
	// 1，生成最終洗牌種子
	const std::string finallySeed = GenFinallySeed("o0fGqmei", "mgFqmN5z", "", "",
		"", "", "", "","6511474087809777665");

	// 2，根據各種不同玩法認定不同參數
	// 2.1，BlackJack玩法，6副牌，最大牌面值13
	// PokerPrintShuffle(finallySeed, 312, 13);

	// 2.2，股子玩法，使用3個股子
	// DiceShuff(finallySeed, 3);

	// 2.2，百家樂玩法，6副牌，最大牌面值13
	// PokerPrintShuffle(finallySeed, 312, 13);

	getchar();
	return 0;
}
