/*
	LPCJTAG, a GDB remote JTAG connector,

	Copyright (C) 2008 Bjorn Bosell (mithris@misantrop.org)

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright
	   notice, this list of conditions and the following disclaimer.
	2. Redistributions in binary form must reproduce the above copyright
	   notice, this list of conditions and the following disclaimer in the
	   documentation and/or other materials provided with the distribution.
	3. The name of the author may not be used to endorse or promote products
	   derived from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
	IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
	OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, 
	INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
	NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
	THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef _TARGET_H_
#define _TARGET_H_

#include "jtag.h"
#include <stdlib.h>


class target
{
	public:
		target() {
			for (int i = 0; i < max_transactions; ++i) {
				m_trans_free[i] = 0;
			}

		}


		// General overloaded functionality.
		virtual void SetDevice(jtag *device) = 0;
		virtual unsigned long GetRegisterCount() = 0;
		virtual const char *GetRegisterName(unsigned long reg) = 0;
		virtual unsigned long GetRegister(int iReg) = 0;
		virtual void SetRegister(int iReg, unsigned long value) = 0;
		virtual void ReadRegs() = 0;
		virtual void ReadMem(char *pDest, int Address, int size) = 0;
		virtual void WriteMem(char *pData, int Address, int size) = 0;
		virtual void SystemReset() = 0;
		virtual void Halt() = 0;
		virtual bool Halted() = 0;
		virtual void Continue() = 0;
		virtual void MaskInt() = 0;
		virtual void UnMaskInt() = 0;
		virtual void HandleException() = 0;
		virtual void SingleStep() = 0;
		virtual void KillMMU() = 0;
		virtual void setFlag(unsigned long flag, unsigned long value) = 0;
		virtual void CommChannelWrite(unsigned long data) = 0;
		virtual void CommChannelWriteStream(unsigned char *indata, int bytes) = 0;
		virtual void CommChannelReadStream(unsigned char *indata, int bytes) = 0;
		virtual unsigned long CommChannelRead() = 0;

		// Breakpoints etc.
		virtual void SetBreakpoint(unsigned long Address) = 0;
		virtual void ClearBreakpoint(unsigned long Address) = 0;
		virtual void SetWatchPoint(unsigned long Address, unsigned long size, unsigned long count, bool bWrite = true) = 0;
		virtual void ClearWatchPoint(unsigned long Address) = 0;
		virtual void GetSWBreakpointInstruction(unsigned char *data, int size) = 0;

		// MMU/Caches
		virtual unsigned long TranslateAddress(unsigned long MVA) = 0;
		virtual void FlushDCache() = 0;
		virtual void FlushICache() = 0;


		typedef enum {
			eFeatureDCC
		} EFeature;


		typedef enum {
			eSpecialRegPC,
			eSpecialRegSP,
			eSpecialRegFP
		} ESpecialReg;

        /** Translates a special register name to a target specific
         * register index.
         * (ex. eSpecRegPC would be 15 on ARM)
         * 
         * @param reg   Special register
		 * 
         * @return int  Target register index.
		 */
		virtual int	GetSpecialRegister(ESpecialReg reg) = 0;

        /** GDB has some funny notion of having the FP registers before
         *  the CPSR etc, so it's not a linear list from r0->cpsr
		 * 
         * @param ireg  GDB register index.
		 * 
         * @return int  Actual register index.
		 */
		virtual int	MapGDBRegister(int ireg) = 0;

        /** Checks if a target supports a certain feature.
		 * 
         * @param feature   Feature in question
		 * 
         * @return bool     True if the feature is supported, otherwise
         *         false.
		 */
		virtual bool HasFeature(EFeature feature) = 0;


        /** Enters DCC mode. (Direct Communication Channel)
         *  On ARM processors this uploads and starts a small
         *  program at 'address' which can read data directly
         *  from the JTAG port, making memory transfers alot
         *  quicker.
		 * 
         * @param address
		 */
		virtual void EnterDCCMode(unsigned long address) = 0;


        /** Leaves DCC mode, stops the core and restores the
         *  register contents.
		 * 
		 */
		virtual void LeaveDCCMode() = 0;

	protected:

		const static int	max_transactions = 8;
		jtag_transaction	m_transactions[max_transactions];
		char				m_trans_free[max_transactions];

		jtag_transaction *
		getTrans()
		{
			for (int i = 0; i < max_transactions; ++i) {
				if (0 == m_trans_free[i]) {
					m_trans_free[i] = 1;
					return &m_transactions[i];
				}
			}
			while(1);
			return NULL;
		}

		void
		relTrans(jtag_transaction *trans)
		{
			for (int i = 0; i < max_transactions; ++i) {
				if (&m_transactions[i] == trans) {
					m_trans_free[i] = 0;
					return;
				}
			}
			while(1);
		}



};



#endif // _TARGET_H_
