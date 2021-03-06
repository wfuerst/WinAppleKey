#include "driver.h"

static BOOLEAN FakeFnActive = 0;

void ProcessMagicKbBlock(PBRB pbrb)
{
	PUCHAR buf = (PUCHAR)pbrb->BrbL2caAclTransfer.Buffer;
	ULONG bufSize = pbrb->BrbL2caAclTransfer.BufferSize;
	KdPrintBuffer("ProcessMagicKbBlock(): In:  ", buf, bufSize);

	//Process LCtrl modifier and translate to FakeFn key
	if ((buf[2] & 0x1) && !buf[4])
		FakeFnActive = TRUE;
	else if (!buf[4])
		FakeFnActive = FALSE;
	buf[2] &= ~HidLCtrlMask; //Clear LCtrl

	//Process special key modifier
	if (buf[10]) 
	{
		if (buf[10] & 0x1) //Eject (translate to Del)
			buf[4] = HidDel;
		if (buf[10] & 0x2) //Fn (translate to LCtrl)
			buf[2] |= HidLCtrlMask; //Set LCtrl

		buf[10] = 0; //Clear special key so that the buffer can be understood by hidclass up the stack	
	}

	//Process Fn+[key] combination 
	if (FakeFnActive && (buf[4] || buf[2])) 
	{
		if (buf[4] == HidLeft) buf[4] = HidHome; 
		else if (buf[4] == HidRight) buf[4] = HidEnd;
		else if (buf[4] == HidUp) buf[4] = HidPgUp;
		else if (buf[4] == HidDown) buf[4] = HidPgDown;
		else if (buf[4] == HidEnter) buf[4] = HidInsert;
		else if (buf[4] == HidF1) buf[4] = HidF13;
		else if (buf[4] == HidF2) buf[4] = HidF14;
		else if (buf[4] == HidF3) buf[4] = HidF15;
		else if (buf[4] == HidF4) buf[4] = HidF16;
		else if (buf[4] == HidF5) buf[4] = HidF17;
		else if (buf[4] == HidF6) buf[4] = HidF18;
		else if (buf[4] == HidF7) buf[4] = HidF19;
		else if (buf[4] == HidF8) buf[4] = HidF20;
		else if (buf[4] == HidF9) buf[4] = HidF21;
		else if (buf[4] == HidF10) buf[4] = HidF22;
		else if (buf[4] == HidF11) buf[4] = HidF23;
		else if (buf[4] == HidF12) buf[4] = HidF24;
		else if (buf[4] == HidKeyP) buf[4] = HidPrtScr;
		else if (buf[4] == HidKeyB) buf[4] = HidPauseBreak;
		else if (buf[4] == HidKeyS) buf[4] = HidScrLck;
		else if (buf[2] & HidLCtrlMask)
		{
			buf[2] &= ~HidLCtrlMask; //Clear LCtrl
			buf[2] |= HidRCtrlMask; //Set RCtrl
		}
		else
			RtlZeroMemory(buf + 2, pbrb->BrbL2caAclTransfer.BufferSize - 2);
	}

	KdPrintBuffer("ProcessMagicKbBlock(): Out: ", buf, bufSize);
}