#include "MFST.h"

int FST_TRACE_n = -1;
char rbuf[205], sbuf[205], lbuf[1024];

#define NS(n) GRB::Rule::Chain::N(n)
#define TS(n) GRB::Rule::Chain::T(n)
#define ISNS(n) GRB::Rule::Chain::isN(n)

#define MFST_TRACE1(log)	log <<std::setw(4)<<std::left<<++FST_TRACE_n<<": "\
						    <<std::setw(20)<<std::left << rule.getCRule(rbuf, nrulechain)\
						    <<std::setw(30)<<std::left << getCLenta(lbuf, lenta_position)\
						    <<std::setw(20)<<std::left << getCSt(sbuf)\
						    <<std::endl;	
#define MFST_TRACE2(log)	log <<std::setw(4)<<std::left<< FST_TRACE_n<<": "\
						    <<std::setw(20)<<std::left << " "\
						    <<std::setw(30)<<std::left << getCLenta(lbuf, lenta_position)\
						    <<std::setw(20)<<std::left << getCSt(sbuf)\
						    <<std::endl;
#define MFST_TRACE3(log)	log <<std::setw(4)<<std::left<<++FST_TRACE_n<<": "\
						    <<std::setw(20)<<std::left << " "\
						    <<std::setw(30)<<std::left << getCLenta(lbuf, lenta_position)\
						    <<std::setw(20)<<std::left << getCSt(sbuf)\
						    <<std::endl;
#define MFST_TRACE4(log,c)	log <<std::setw(4)<<std::left<< ++FST_TRACE_n<<": "\
						    <<std::setw(20)<<std::left<<c<<std::endl;
#define MFST_TRACE5(log,c) log <<std::setw(4)<<std::left<< FST_TRACE_n<<": "\
						    <<std::setw(20)<<std::left<<c<<std::endl;
#define MFST_TRACE6(log,c,k) log <<std::setw(4)<<std::left<< FST_TRACE_n<<": "\
						    <<std::setw(20)<<std::left<<c<<k<<std::endl;

#define MFST_TRACE7(log) log <<std::setw(4)<<std::left<< state.lenta_position<<": "\
				    <<std::setw(20)<<std::left<<rule.getCRule(rbuf,state.nrulechain)<<std::endl;


namespace MFST
{
	MfstState::MfstState()
	{
		lenta_position = 0;
		nrule = -1;
		nrulechain = -1;
	}

	MfstState::MfstState(short pposition, MFSTSTACK pst, short pnrulechain)
	{
		lenta_position = pposition;
		st = pst;
		nrulechain = pnrulechain;
	}

	MfstState::MfstState(short pposition, MFSTSTACK pst, short pnrule, short pnrulechain)
	{
		lenta_position = pposition;
		st = pst;
		nrule = pnrule;
		nrulechain = pnrulechain;
	}

	Mfst::MfstDiagnosis::MfstDiagnosis()
	{
		lenta_position = -1;
		rc_step = SURPRISE;
		nrule = -1;
		nrule_chain = -1;
	}

	Mfst::MfstDiagnosis::MfstDiagnosis(short plenta_position, RC_STEP prc_step,
		short pnrule, short pnrule_chain)
	{
		lenta_position = plenta_position;
		rc_step = prc_step;
		nrule = pnrule;
		nrule_chain = pnrule_chain;
	}

	Mfst::Mfst() {
		lenta = 0;
		lenta_size = lenta_position = 0;
	}

	Mfst::Mfst(LEX::LEX plex, GRB::Greibach pgreibach)
	{
		greibach = pgreibach;
		lex = plex;
		lenta = new short[lenta_size = lex.lextable.size];
		for (int k = 0; k < lenta_size; ++k)lenta[k] = TS(lex.lextable.table[k].lexema[0]);
		lenta_position = 0;
		st.push(greibach.stbottomT);
		st.push(greibach.startN);
		nrulechain = -1;
	}

	Mfst::RC_STEP Mfst::step(std::ofstream* out)
	{
		RC_STEP rc = SURPRISE;
		if (lenta_position < lenta_size)
		{
			if (ISNS(st.top()))
			{
				GRB::Rule rule;
				if ((nrule = greibach.getRule(st.top(), rule)) >= 0)
				{
					GRB::Rule::Chain chain;
					if ((nrulechain = rule.getNextChain(lenta[lenta_position], chain, nrulechain + 1)) >= 0)
					{
						MFST_TRACE1(*out)
							savestate(out);
						st.pop(); 
						push_chain(chain);
						rc = NS_OK;
						MFST_TRACE2(*out)
					}
					else
					{
						MFST_TRACE4(*out, "TNS_NORULECHAIN/NS_NORULE")
						savediagnosis(NS_NORULECHAIN);
						rc = reststate(out) ? NS_NORULECHAIN : NS_NORULE;
					}
				}
				else rc = NS_ERROR;
			}
			else if (st.top() == lenta[lenta_position])
			{
				lenta_position++;
				st.pop();
				nrulechain = -1;
				rc = TS_OK;
				MFST_TRACE3(*out)
			}
			else { MFST_TRACE4(*out, "TS_NOK/NS_NORULECHAIN") rc = reststate(out) ? TS_NOK : NS_NORULECHAIN; }
		}
		else { rc = LENTA_END; MFST_TRACE4(*out, "LENTA_END") }
		return rc;
	}


	bool Mfst::push_chain(GRB::Rule::Chain chain)
	{
		for (int k = chain.size - 1; k >= 0; k--)st.push(chain.nt[k]);
		return true;
	}

	bool Mfst::savestate(std::ofstream* out)
	{
		storestate.push(MfstState(lenta_position, st, nrule, nrulechain));
		MFST_TRACE6(*out, "SAVESTATE:", storestate.size());
		return true;
	}
	bool Mfst::reststate(std::ofstream* out)
	{
		bool rc = false;
		MfstState state;
		if (rc = (storestate.size() > 0))
		{
			state = storestate.top();
			lenta_position = state.lenta_position;
			st = state.st;
			nrule = state.nrule;
			nrulechain = state.nrulechain;
			storestate.pop();
			MFST_TRACE5(*out, "RESSTATE")
			MFST_TRACE2(*out)
		}
		return rc;
	}

	bool Mfst::savediagnosis(RC_STEP prc_step)
	{
		bool rc = false;
		short k = 0;
		while (k < MFST_DIAGN_NUMBER && lenta_position <= diagnosis[k].lenta_position)k++;
		if (rc = (k < MFST_DIAGN_NUMBER))
		{
			diagnosis[k] = MfstDiagnosis(lenta_position, prc_step, nrule, nrulechain);
			for (short j = k + 1; j < MFST_DIAGN_NUMBER; ++j) diagnosis[j].lenta_position = -1;
		}
		return rc;
	}

	bool Mfst::start(std::ofstream* out)
	{
		bool rc = false;
		RC_STEP rc_step = SURPRISE;
		char buf[MFST_DIAGN_MAXSIZE];
		rc_step = step(out);
		while (rc_step == NS_OK || rc_step == NS_NORULECHAIN || rc_step == TS_OK ||
			rc_step == TS_NOK)rc_step = step(out);

		switch (rc_step)
		{
		case LENTA_END:  MFST_TRACE4(*out, "------>LENTA_END")
			*out << "-------------------------------------------------------------------  -----" << std::endl;
			sprintf_s(buf, MFST_DIAGN_MAXSIZE, "%d ����� ����� %d, �������������� ������ �������� ��� ������" , 0, lenta_size);
			*out << std::setw(4) << std::left << 0 << ": ����� ����� " << lenta_size << ", �������������� ������ �������� ��� ������" << std::endl;
			rc = true;
			break;
		case NS_NORULE:   MFST_TRACE4(*out,"------->NS_NORULE")
			* out << "-------------------------------------------------------------------  -----" << std::endl;
			std::cout << getDiagnosis(0, buf) << std::endl;
			std::cout << getDiagnosis(1, buf) << std::endl;
			std::cout << getDiagnosis(2, buf) << std::endl;

			*out << getDiagnosis(0, buf) << std::endl;
			*out << getDiagnosis(1, buf) << std::endl;
			*out << getDiagnosis(2, buf) << std::endl;
			break;
		case NS_NORULECHAIN: MFST_TRACE4(*out,"----->NS_NORULECHAIN")break;
		case NS_ERROR:		 MFST_TRACE4(*out,"----->NS_ERROR")break;
		case SURPRISE:		 MFST_TRACE4(*out,"----->SURPRISE")break;
		}

		return rc;
	}

	char* Mfst::getCSt(char* buf)
	{
		MFSTSTACK temp = st;
		for (int k = st.size() - 1; k >= 0; --k)
		{
			short p = temp.top();
			temp.pop();
			buf[st.size() - 1 - k] = GRB::Rule::Chain::alphabet_to_char(p);
		}
		buf[st.size()] = 0x00;
		return buf;
	}

	char* Mfst::getCLenta(char* buf, short pos, short n)
	{
		short i, k = (pos + n < lenta_size) ? pos + n : lenta_size;
		for (i = pos; i < k; ++i)buf[i - pos] = GRB::Rule::Chain::alphabet_to_char(lenta[i]);
		buf[i - pos] = 0x00;
		return buf;
	}

	char* Mfst::getDiagnosis(short n, char* buf)
	{
		char* rc = (char*)"";
		int errid = 0;
		int lpos = -1;
		if (n < MFST_DIAGN_NUMBER && (lpos = diagnosis[n].lenta_position) >= 0)
		{
			errid = greibach.getRule(diagnosis[n].nrule).iderror;
			Error::ERROR err = Error::geterror(errid);
			sprintf_s(buf, MFST_DIAGN_MAXSIZE, "%d: ������ %d, %s", err.id, lex.lextable.table[lpos].sn, err.message);
			rc = buf;
		}
		return rc;
	}

	void Mfst::printrules(std::ofstream* out)
	{
		MfstState state;
		GRB::Rule rule;
		//std::stack<MfstState> temp = storestate;
		for (unsigned short k = 0; k < storestate.size(); ++k)
		{
			//state = temp.top();
			//temp.pop();
			state = storestate[k];
			rule = greibach.getRule(state.nrule);
			MFST_TRACE7(*out)
		}
	}

	bool Mfst::savededucation()
	{
		MfstState state;
		GRB::Rule rule;
		deducation.nrules = new short[deducation.size = storestate.size()];
		deducation.nruleschains = new short[deducation.size];
		//std::stack<MfstState> temp = storestate;
		for (unsigned short k = 0; k < storestate.size(); ++k)
		{
			//state = temp.top();
			//temp.pop();
			state = storestate[k];
			deducation.nrules[k] = state.nrule;
			deducation.nruleschains[k] = state.nrulechain;
		}
		return true;
	}

}