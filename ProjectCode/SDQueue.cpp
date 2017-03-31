#include "SDQueue.h"

SDQueue::SDQueue()
/*******************************************************************************************
 *
 * 	Constructor sets the queue in a state that flags it as empty (iBack = -1)
 *
 *******************************************************************************************/
{
	iFront = 0;
	iBack = -1;
}

bool SDQueue::enqueue(sd_line data)
/*******************************************************************************************
 *
 * 	adds the message to the queue. Returns false if the queue was full and could not be added
 *
 *******************************************************************************************/
{
	bool success;
	if(iBack != iFront)
	{
		//special case for very first entry
		if(iBack == -1)
		{
			buff[0] = data;
			iBack = 1;
			success = true;
		}
		else
		{
			buff[iBack] = data;
			iBack++;
			if(iBack == QUEUELENGTH)
			{
				iBack = 0;
			}
		}
	}
	else//queue full
	{
		success = false;
	}
	return success;
}
sd_line SDQueue::dequeue(bool &ok)
/*******************************************************************************************
 *
 * 	removes the top message from the queue. Sets the bool flag to false if the queue was empty
 * 	and could not remove anymore messages
 *
 *******************************************************************************************/
{
	ok = false;
	sd_line returnLine;
	if(!isEmpty())
	{
		returnLine = buff[iFront];
		iFront++;
		if(iFront == QUEUELENGTH)
		{
			iFront = 0;
		}
		if(iFront == iBack)//queue empty. reset
		{
			iBack = -1;
			iFront = 0;
		}
		ok = true;

	}
	else
	{
		ok = false;
	}
	return returnLine;
}

bool SDQueue::isEmpty()
/*******************************************************************************************
 *
 * 	Returns whether or not the queue is empty
 *
 *******************************************************************************************/
{
	return (iBack == -1);
}
