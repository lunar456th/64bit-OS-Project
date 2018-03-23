#include "RTC.h"
#include "AssemblyUtility.h"

// CMOS 메모리에 저장한 현재 시간값을 읽음
void kReadRTCTime(BYTE * pbHour, BYTE * pbMinute, BYTE * pbSecond)
{
	BYTE bData;

	// CMOS 레지스터에 HOUR어드레스 올리고 값 읽어와서 변환
	kOutPortByte(RTC_CMOSADDRESS, RTC_ADDRESS_HOUR);
	bData = kInPortByte(RTC_CMOSDATA);
	*pbHour = RTC_BCDTOBINARY(bData);

	// CMOS 레지스터에 MINUTE어드레스 올리고 값 읽어와서 변환
	kOutPortByte(RTC_CMOSADDRESS, RTC_ADDRESS_MINUTE);
	bData = kInPortByte(RTC_CMOSDATA);
	*pbMinute = RTC_BCDTOBINARY(bData);

	// CMOS 레지스터에 MINUTE어드레스 올리고 값 읽어와서 변환
	kOutPortByte(RTC_CMOSADDRESS, RTC_ADDRESS_SECOND);
	bData = kInPortByte(RTC_CMOSDATA);
	*pbSecond = RTC_BCDTOBINARY(bData);
}

// CMOS 메모리에 저장한 현재 날짜값을 읽음
void kReadRTCDate(WORD * pwYear, WORD * pwMonth, WORD * pwDayOfMonth, BYTE * pbDayOfWeek)
{
	WORD bData;

	// CMOS 레지스터에 YEAR어드레스 올리고 값 읽어와서 변환
	kOutPortByte(RTC_CMOSADDRESS, RTC_ADDRESS_YEAR);
	bData = kInPortByte(RTC_CMOSDATA);
	*pwYear = RTC_BCDTOBINARY(bData) + 2000;

	// CMOS 레지스터에 MONTH어드레스 올리고 값 읽어와서 변환
	kOutPortByte(RTC_CMOSADDRESS, RTC_ADDRESS_MONTH);
	bData = kInPortByte(RTC_CMOSDATA);
	*pwMonth = RTC_BCDTOBINARY(bData);

	// CMOS 레지스터에 DAYOFMONTH어드레스 올리고 값 읽어와서 변환
	kOutPortByte(RTC_CMOSADDRESS, RTC_ADDRESS_DAYOFMONTH);
	bData = kInPortByte(RTC_CMOSDATA);
	*pwDayOfMonth = RTC_BCDTOBINARY(bData);

	// CMOS 레지스터에 DAYOFWEEK어드레스 올리고 값 읽어와서 변환
	kOutPortByte(RTC_CMOSADDRESS, RTC_ADDRESS_DAYOFWEEK);
	bData = kInPortByte(RTC_CMOSDATA);
	*pbDayOfWeek = RTC_BCDTOBINARY(bData);
}

char * kConvertDayOfWeekToString(BYTE bDayOfWeek)
{
	static char * vpcDayOfWeekString[8] = { // 요일 배열
			"Error",
			"Monday",
			"Tuesday",
			"Wednesday",
			"Thursday",
			"Friday",
			"Saturday",
			"Sunday",
	};

	// 예외처리
	if(bDayOfWeek >= 8)
	{
		return vpcDayOfWeekString[0]; // "ERROR" 반환
	}

	return vpcDayOfWeekString[bDayOfWeek];
}

