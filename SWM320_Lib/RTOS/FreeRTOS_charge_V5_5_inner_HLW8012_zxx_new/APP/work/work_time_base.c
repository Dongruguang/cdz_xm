#include "work.h"


#define PRT_LOG(X...)	sysprt->alog(X)	
//#define PRT_LOG(X...)	printf(X)	

// 将小时和分钟转换为总分钟数
int to_minutes( uint8_t hour,uint8_t min ) {
		int ret = hour;
		ret = ret * 60 + min;
    return ret;
}



// 计算时间段的重叠分钟数
//start_period 时间段开始点 				end_period 时间段结束点
//start 	 需要处理的时间开始点，		end   需要处理的时间结束点
int calculate_overlap( slot_t period0 ,slot_t period1 ) 
{
    int period_start 	= period0.start;
    int period_end 		= period0.end;

    int time_start 		= period1.start;
    int time_end 			= period1.end;
	
    // 处理跨天情况
    if ( time_start> period_end ) 
		{
      	period_start 	+= 24 * 60; 	// 增加一天的分钟数
        period_end 		+= 24 * 60; 	// 增加一天的分钟数
    }
		
    if (period_end <= time_start || period_start >= time_end) {
        return 0; // 没有重叠
    }
		
    // 计算重叠的开始和结束
    int overlap_start = (period_start > time_start) ? period_start : time_start;
    int overlap_end = (period_end < time_end) ? period_end : time_end;

    return overlap_end - overlap_start; // 返回重叠的分钟数
}

//判断处于那个时间段
int get_time_segments( slot_t* list,int len,int hour,int min )
{
	int ret = 0;
	uint32_t time = 0;
	uint32_t start = 0;
	uint32_t end = 0;
	time = hour*60+min;
	for( int seg=0;seg<len;seg++ )
	{
		start = list[seg].start;
		end = list[seg].end;
		if( (start <= time) &&( time <= end ) )
		{
			ret = seg;
			break;
		}
		
	}
	
	return ret;
	
}


// 判断是否是闰年
int isLeapYear(uint16_t year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// 获取某个月的天数
uint8_t getDaysInMonth(uint16_t year, uint8_t month) {
    if (month == 2) {
        return isLeapYear(year) ? 29 : 28;
    }
    const uint8_t daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return daysInMonth[month];
}

typedef struct{
	uint32_t Year;
	uint32_t Month;
	uint32_t Date;
	uint32_t Day;			
	uint32_t Hour;
	uint32_t Minute;
	uint32_t Second;
}dt_t;

void addTime( rtc_dt_t *dateTime, int years, int months, int days, int hours, int minutes, int seconds) 
{
	
	dt_t m_dt;
	dt_t *DT = &m_dt;
	
	//因为 rtc_dt_t 的成员 变量长度有限，可能要溢出，需要借助临时变量  DT  来处理。
	DT->Year 		= dateTime->Year;
	DT->Month 	= dateTime->Month;
	DT->Date 		= dateTime->Date;
	DT->Day 		= dateTime->Day;
	DT->Hour 		= dateTime->Hour;
	DT->Minute 	= dateTime->Minute;
	DT->Second 	= dateTime->Second;	
	
	// 添加秒
	DT->Second += seconds;
	while (DT->Second >= 60) {
			DT->Second -= 60;
			DT->Minute++;
	}
	
	// 添加分钟
	DT->Minute += minutes;
	while (DT->Minute >= 60) {
			DT->Minute -= 60;
			DT->Hour++;
	}
	
	// 添加小时
	DT->Hour += hours;
	while (DT->Hour >= 24) {
			DT->Hour -= 24;
			DT->Date++;
	}

	// 添加天
	DT->Date += days;
	while (DT->Date > getDaysInMonth(DT->Year, DT->Month)) {
			DT->Date -= getDaysInMonth(DT->Year, DT->Month);
			DT->Month++;
			if (DT->Month > 12) {
					DT->Month = 1;
					DT->Year++;
			}
	}

	// 添加月
	DT->Month += months;
	while (DT->Month > 12) {
			DT->Month -= 12;
			DT->Year++;
	}

	// 添加年
	DT->Year += years;
	
	//处理完，转存
	dateTime->Year 		= DT->Year;
	dateTime->Month 	= DT->Month;
	dateTime->Date 		= DT->Date;
	dateTime->Day 		= DT->Day;
	dateTime->Hour 		= DT->Hour;
	dateTime->Minute 	= DT->Minute;
	dateTime->Second 	= DT->Second;
}


// 函数：将日期时间转换为总分钟数
uint32_t datetime_to_minutes( rtc_dt_t dt ) 
{
    uint32_t total_minutes = 0;

    // 计算完整年份的分钟数
    for (int year = 0; year < dt.Year; year++) {
        total_minutes += 365 * 24 * 60; // 每年分钟数
        // 加上闰年
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
            total_minutes += 24 * 60; // 闰年多一天
        }
    }

    // 计算完整月份的分钟数
    for (int month = 1; month < dt.Month; month++) {
        total_minutes += getDaysInMonth(dt.Year, month) * 24 * 60; // 每月分钟数
    }
		
    // 加上当前日期的分钟数
    total_minutes += (dt.Date - 1) * 24 * 60; // 从1号到当前日期的分钟数
    total_minutes += dt.Hour * 60;            // 当前小时的分钟数
    total_minutes += dt.Minute;                // 当前分钟数

    return total_minutes;
}

// 函数：计算过去了多少分钟
uint32_t calculate_minutes_difference( rtc_dt_t current, rtc_dt_t past) 
{
    uint32_t current_minutes = datetime_to_minutes(current);
    uint32_t past_minutes = datetime_to_minutes(past);
    return current_minutes - past_minutes; // 返回差值
}



void dump_rtc_by_str( char* time )
{
	int i=0;
	uint8_t tbuf[20];
	uint32_t year,month,date,hour,min,second;

	//年 0123
	memset( tbuf,0,sizeof(tbuf) );
	for( i=0;i<4;i++ )	{tbuf[ i ] = time[ 0 + i ];}
	year = atol( (char*)tbuf );
	
	//月 45
	memset( tbuf,0,sizeof(tbuf) );
	for( i=0;i<2;i++ )	{tbuf[ i ] = time[ 4 + i ];}
	month = atol( (char*)tbuf );
	
	//日 67
	memset( tbuf,0,sizeof(tbuf) );
	for( i=0;i<2;i++ )	{tbuf[ i ] = time[ 6 + i ];}
	date = atol( (char*)tbuf );
	
	//时 89
	memset( tbuf,0,sizeof(tbuf) );
	for( i=0;i<2;i++ )	{tbuf[ i ] = time[ 8 + i ];}
	hour = atol( (char*)tbuf );
	
	//分 10 11
	memset( tbuf,0,sizeof(tbuf) );
	for( i=0;i<2;i++ )	{tbuf[ i ] = time[ 10 + i ];}
	min = atol( (char*)tbuf );
	
	//秒 12 13
	memset( tbuf,0,sizeof(tbuf) );
	for( i=0;i<2;i++ )	{tbuf[ i ] = time[ 12 + i ];}
	second = atol( (char*)tbuf );
	
	sysprt->alog("%d/%d/%d - %d:%d:%d\r\n",year,month,date,hour,min,second );
					
	rtc_dt.Year = year;
	rtc_dt.Month = month;
	rtc_dt.Date = date;
	rtc_dt.Hour = hour;
	rtc_dt.Minute = min;
	rtc_dt.Second = second;
	
	rtc->stop();
	rtc->init( &rtc_dt,0,1 );
	rtc->start();
	
}




