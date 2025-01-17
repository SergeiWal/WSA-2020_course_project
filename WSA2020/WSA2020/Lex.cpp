#include "Lex.h"

namespace LEX
{
	LexExample::LexExample(char firstCh, FST::FST nfst, LexType tp)
	{
		firstSymbol = firstCh;
		fst = nfst;
		type = tp;
	}

	LexType LAnaliz( In::word word)
	{
		LexExample tokensArray[TOKENS_ARRAY_SIZE] = TOKENS_ARRAY;

		unsigned char* str = UchVectorToString(word.value);
		

		for (int i = 0; i < TOKENS_ARRAY_SIZE; ++i)
		{
			if (str[0] == tokensArray[i].firstSymbol)
			{
				tokensArray[i].fst.string = str;
				if (FST::execute(tokensArray[i].fst))
				{
					return tokensArray[i].type;
				}
			}
		}

		FST::FST o_fst
		{
			str,
			OPERATION
		};
		if (FST::execute(o_fst))return LexType::O;

		FST::FST id_fst
		{
			str,
			ID
		};
		if (FST::execute(id_fst))return LexType::I;

		throw ERROR_THROW_IN(123, word.line, word.begin);
	}

	unsigned char* UchVectorToString(const std::vector<unsigned char>& word)
	{
		unsigned char* str = new unsigned char[word.size()];
		for (int i = 0; i < word.size(); ++i)str[i] = word[i];
		str[word.size()] = STR_END;
		return str;
	}

	void TableFill(In::IN in, LEX& lex)
	{
		LexType lextype;
		std::stack<std::string> visibleRegions;
		visibleRegions.push(GLOBAL_VISIBLE);

		IT::Entry* itNewEntry;

		IT::IDDATATYPE currentIdDataType = IT::IDDATATYPE::NONE;
		IT::IDTYPE currentIdType = IT::IDTYPE::NONE;

		bool isCycle = false;
		bool isMain = false;
		bool isExtr = false;

		for (int i = 0; i<in.text.size(); ++i)
		{
			LT::Entry* ltNewEntry = new LT::Entry;
			ltNewEntry->sn = in.text[i].line;
			ltNewEntry->idxTI = LT_IT_NULLIDX;

			if (in.text[i].value.size() == 1 && lex.one_symbol_lexems.find(in.text[i].value[0]) != lex.one_symbol_lexems.end())
			{
				if (in.text[i].value[0] == LEX_CALL)currentIdType = IT::IDTYPE::C;
				ExitFromVisibleRegion(visibleRegions, in.text[i].value[0], isCycle, isExtr);
				SetNewLtNodeValue(*ltNewEntry, in.text[i].value[0]);
			}
			else
			{
				lextype = LAnaliz(in.text[i]);
				switch (lextype)
				{
				case LexType::E:
					isExtr = true;
					currentIdType = IT::IDTYPE::E;
					SetNewLtNodeValue(*ltNewEntry, LEX_EXTR);
					break;
				case LexType::F:
					currentIdType = IT::IDTYPE::F;
					SetNewLtNodeValue(*ltNewEntry, LEX_FUNCTION);
					break;
				case LexType::I:
					SetNewLtNodeValue(*ltNewEntry, LEX_ID);

					itNewEntry = new IT::Entry(currentIdType,currentIdDataType,visibleRegions.top(), in.text[i].value);
					IsParametrSet(*itNewEntry, in.text[i + 1].value[0]);

					ltNewEntry->idxTI = IT::IsId(lex.idtable, *itNewEntry);
					if (itNewEntry->idtype == IT::IDTYPE::V && ltNewEntry->idxTI != TI_NULLIDX)
						throw ERROR_THROW_IN(127, in.text[i].line, in.text[i].begin);
					if ((itNewEntry->idtype == IT::IDTYPE::F || itNewEntry->idtype == IT::IDTYPE::P) && ltNewEntry->idxTI != TI_NULLIDX)
						throw ERROR_THROW_IN(127, in.text[i].line, in.text[i].begin);
					if (itNewEntry->idtype == IT::IDTYPE::V && itNewEntry->visibilityRegion == GLOBAL_VISIBLE)
						throw ERROR_THROW_IN(309, in.text[i].line, in.text[i].begin);

					if (ltNewEntry->idxTI == TI_NULLIDX) 
					{
						if (itNewEntry->idtype == IT::IDTYPE::NONE)
							throw ERROR_THROW_IN(125, in.text[i].line, in.text[i].begin);
						if (itNewEntry->iddatatype == IT::IDDATATYPE::NONE && itNewEntry->idtype != IT::IDTYPE::P && itNewEntry->idtype != IT::IDTYPE::E)
							throw ERROR_THROW_IN(126, in.text[i].line, in.text[i].begin);

						SetDefaultValue(*itNewEntry);
						ltNewEntry->idxTI = lex.idtable.size;
						itNewEntry->idxfirstLE = lex.lextable.size;
						IT::Add(lex.idtable, *itNewEntry);
					} 
					
					if (itNewEntry->idtype == IT::IDTYPE::F || itNewEntry->idtype == IT::IDTYPE::P ||
						itNewEntry->idtype == IT::IDTYPE::E)
								visibleRegions.push(itNewEntry->id);

					currentIdDataType = IT::IDDATATYPE::NONE;
					currentIdType = IT::IDTYPE::NONE;
					break;
				case LexType::L:
					SetNewLtNodeValue(*ltNewEntry, LEX_LITERAL);
					currentIdType = IT::IDTYPE::L;
					itNewEntry = new IT::Entry;
					SetLiteralValue(*itNewEntry, in.text[i]);
					ltNewEntry->idxTI = IT::IsId(lex.idtable, *itNewEntry);
					if (ltNewEntry->idxTI == TI_NULLIDX)
					{
						SetLiteralName(*itNewEntry, i);
						ltNewEntry->idxTI = lex.idtable.size;
						itNewEntry->idxfirstLE = lex.lextable.size;
						IT::Add(lex.idtable, *itNewEntry);
					}
					currentIdDataType = IT::IDDATATYPE::NONE;
					currentIdType = IT::IDTYPE::NONE;
					break;
				case LexType::M:
					visibleRegions.push(MAIN_VISIBLE);
					SetNewLtNodeValue(*ltNewEntry, LEX_MAIN);
					if (!isMain)isMain = true;
					else throw ERROR_THROW_IN(132, in.text[i].line, in.text[i].begin);
					break;
				case LexType::O:
					currentIdDataType = IT::IDDATATYPE::NONE;
					currentIdType = IT::IDTYPE::NONE;
					itNewEntry = new IT::Entry;
					itNewEntry->idtype = IT::IDTYPE::O;
					SetOperatorValue(*itNewEntry, in.text[i].value[0]);
					ltNewEntry->idxTI = IT::IsId(lex.idtable, *itNewEntry);
					if (ltNewEntry->idxTI == TI_NULLIDX)
					{
						ltNewEntry->idxTI = lex.idtable.size;
						itNewEntry->idxfirstLE = lex.lextable.size;
						IT::Add(lex.idtable, *itNewEntry);
					}
					SetNewLtNodeValue(*ltNewEntry, LEX_OPERATION);
					break;
				case LexType::P:
					currentIdType = IT::IDTYPE::P;
					SetNewLtNodeValue(*ltNewEntry, LEX_PROC);
					break;
				case LexType::R:
					SetNewLtNodeValue(*ltNewEntry, LEX_RETURN);
					break;
				case LexType::T:
					currentIdDataType = GetDataType(in.text[i].value);
					SetNewLtNodeValue(*ltNewEntry, LEX_TYPE);
					break;
				case LexType::V:
					currentIdType = IT::IDTYPE::V;
					SetNewLtNodeValue(*ltNewEntry, LEX_VAR);
					break; 
				case LexType::C:
					isCycle = true;
					SetNewLtNodeValue(*ltNewEntry, LEX_CYCLE);
					break;
				case LexType::W:
					SetNewLtNodeValue(*ltNewEntry, LEX_WRITE);
					break;
				default:
					break;
				}

			}
			LT::Add(lex.lextable, *ltNewEntry);
		}

		if (!isMain)throw ERROR_THROW_IN(131, 0, 0);
	}

	void IsParametrSet(IT::Entry& ent, char nextCh)
	{
		if (ent.idtype == IT::IDTYPE::NONE && ent.iddatatype != IT::IDDATATYPE::NONE &&
			(nextCh == LEX_COMA || nextCh == LEX_RIGHTHESIS))
			ent.idtype = IT::IDTYPE::A;
	}

	void ExitFromVisibleRegion(std::stack<std::string>& regions, char currentCh, bool& isCycle, bool& isExtr)
	{
		if (currentCh == LEX_BRACELET && !isCycle)regions.pop();
		else if (currentCh == LEX_BRACELET && isCycle)isCycle = false;
		else if (currentCh == LEX_SEMICOLON && isExtr)
		{
			regions.pop();
			isExtr = false;
		}
	}

	void SetNewLtNodeValue(LT::Entry& entry, char value)
	{
		entry.lexema[0] = value;
		entry.lexema[1] = STR_END;
	}

	IT::IDDATATYPE GetDataType(std::vector<unsigned char> word)
	{
		switch (word[0])
		{
		case 's':
			if(word[1] == 't')return IT::IDDATATYPE::STR;
			if (word[1] == 'h')return IT::IDDATATYPE::INT;
			break;
		case 'b':
			return IT::IDDATATYPE::BL;
			break;
		case 'c':
			return IT::IDDATATYPE::CHR;
			break;
		default:
			return IT::IDDATATYPE::NONE;
			break;
		}
	}

	void SetDefaultValue(IT::Entry& ent)
	{
		switch (ent.iddatatype)
		{
		case IT::IDDATATYPE::BL:
			ent.value.vbool = false;
			break;
		case IT::IDDATATYPE::INT:
			ent.value.vint = 0;
			break;
		case IT::IDDATATYPE::STR:
			ent.value.vstr.len = 0;
			ent.value.vstr.str[0] = STR_END;
			break;
		case IT::IDDATATYPE::CHR:
			ent.value.vchar = '\0';
			break;
		default:
			break;
		}
	}

	void SetLiteralValue(IT::Entry& ent, const In::word& word)
	{
		ent.idtype = IT::IDTYPE::L;
		ent.visibilityRegion = "global";
		if (word.value[0] == '\"')
		{
			int size = 0;
			int i = 1;
			while (word.value[i] != '\"')
			{
				ent.value.vstr.str[size] = word.value[i];
				i++;
				size++;
			}
			if (size > TI_STR_MAXSIZE)throw ERROR_THROW_IN(130, word.line, word.begin);
			ent.value.vstr.len = size;
			ent.value.vstr.str[size] = STR_END;
			ent.iddatatype = IT::IDDATATYPE::STR;
		}
		else if (word.value[0] == '\'')
		{
			ent.iddatatype = IT::IDDATATYPE::CHR;
			ent.value.vchar = word.value[1];
		}
		else if (word.value[0] == 't')
		{
			ent.iddatatype = IT::IDDATATYPE::BL;
			ent.value.vbool = true;
		}
		else if (word.value[0] == 'f')
		{
			ent.iddatatype = IT::IDDATATYPE::BL;
			ent.value.vbool = false;
		}
		else if (word.value[1] == SIGN_BIN_NUMBER)
		{
			ent.iddatatype = IT::IDDATATYPE::INT;
			ent.value.vint = BinToInt(word.value);
			if (ent.value.vint > TI_SHORT_MAXSIZE)throw ERROR_THROW_IN(128, word.line, word.begin);
			if (ent.value.vint < TI_SHORT_NEG_MAXSIZE)throw ERROR_THROW_IN(129, word.line, word.begin);
		}
		else if (word.value[1] == SIGN_OCT_NUMBER)
		{
			ent.iddatatype = IT::IDDATATYPE::INT;
			ent.value.vint = OctToInt(word.value);
			if (ent.value.vint > TI_SHORT_MAXSIZE)throw ERROR_THROW_IN(128, word.line, word.begin);
			if (ent.value.vint < TI_SHORT_NEG_MAXSIZE)throw ERROR_THROW_IN(129, word.line, word.begin);
		}
		
	}

	void SetLiteralName(IT::Entry& ent, int number)
	{
		strcpy_s(ent.id, ID_MAXSIZE, LITERAL_NAME);
		_itoa_s(number, ent.id + 4, ID_MAXSIZE, 10);
	}

	void SetOperatorValue(IT::Entry& ent, char ch)
	{
		switch (ch)
		{
		case '+':
			ent.id[0] = '+';
			ent.id[1] = STR_END;
			break;
		case '-':
			ent.id[0] = '-';
			ent.id[1] = STR_END;
			break;
		case '*':
			ent.id[0] = '*';
			ent.id[1] = STR_END;
			break;
		case '/':
			ent.id[0] = '/';
			ent.id[1] = STR_END;
			break;
		case '%':
			ent.id[0] = '%';
			ent.id[1] = STR_END;
			break;
		case '<':
			ent.id[0] = '<';
			ent.id[1] = STR_END;
			break;
		case '>':
			ent.id[0] = '>';
			ent.id[1] = STR_END;
			break;
		case '=':
			ent.id[0] = '=';
			ent.id[1] = '=';
			ent.id[2] = STR_END;
			break;
		case '!':
			ent.id[0] = '!';
			ent.id[1] = '=';
			ent.id[2] = STR_END;
			break;
		default:
			break;
		}
	}

	int OctToInt(std::vector<unsigned char> word)
	{
		int number = 0;
		int power = 0;
		int i = word.size() - 1;
		while (word[i] != SIGN_OCT_NUMBER)
		{
			number += (word[i] - '0') * pow(8,power);
			power++;
			i--;
		}
		--i;
		if (word[i] == '1')number = -number;
		return number;
	}

	int BinToInt(std::vector<unsigned char> word)
	{
		int number = 0;
		int power = 0;
		int i = word.size() - 1;
		while (word[i] != SIGN_BIN_NUMBER)
		{
			number += (word[i] - '0') * pow(2, power);
			power++;
			i--;
		}
		--i;
		if (word[i] == '1')number = -number; 
		return number;
	}

	void LexTableOut(std::ofstream* out, LT::LexTable lt, IT::IdTable it)
	{
		int number = 0;
		*out << std::setfill(' ') << std::setw(8) << "Number | " << std::setw(10) << 
			"lexema | " << std::setw(10) << "line | " << std::setw(10) << "ITIndex |\n";
		for (int i = 0; i < lt.size; ++i)
		{
			if (lt.table[i].lexema != NULL)
			{
				*out << std::setw(6) << number << " | "<< std::setw(7) << lt.table[i].lexema << " | " << std::setw(7) << lt.table[i].sn << " | ";
				//if (lt.table[i].lexema[0] == SEQ)out << it.table[lt.table[i].idxTI].value.vint;
				//else if (lt.table[i].lexema[0] == LEX_OPERATION)out << '{' << it.table[lt.table[i].idxTI].id << '}';
				if (lt.table[i].lexema[0] == LEX_ID || lt.table[i].lexema[0] == LEX_LITERAL)*out << std::setw(7) << lt.table[i].idxTI << " |\n";
				else *out << std::right << std::setw(10) << " |\n";
				number++;
			}
		}
		
	}

	void IdTableOut(std::ofstream* out, IT::IdTable it)
	{
		*out << std::setfill(' ') << std::setw(10) << "N | " << std::setw(10) << "NAME | " << std::setw(20) << "TYPE | " << std::setw(10) <<
			"REFERENCE | " << std::setw(10) << "VISIBLE |" << std::setw(10) <<
			"DATATYPE | " << std::setw(10) << " VALUE\n";
		for (int i = 0; i < it.size; ++i)
		{
			if (it.table[i].idtype != IT::IDTYPE::O)
			{
				*out << std::setw(8) << i << "|" << std::setw(9) << it.table[i].id << "|";

				switch (it.table[i].idtype)
				{
				case IT::IDTYPE::E:
					*out << std::setw(20) << "extern function |";
					break;
				case IT::IDTYPE::F:
					*out << std::setw(20) << "function name |";
					break;
				case IT::IDTYPE::P:
					*out << std::setw(20) << "procedure name |";
					break;
				case IT::IDTYPE::V:
					*out << std::setw(20) << "variable name |";
					break;
				case IT::IDTYPE::L:
					*out << std::setw(20) << "literal name |";
					break;
				case IT::IDTYPE::A:
					*out << std::setw(20) << "parameter |";
					break;
				default:
					break;
				}

				*out << std::setw(11) << it.table[i].idxfirstLE << "|";
				*out << std::setw(10) << it.table[i].visibilityRegion << "|";


				switch (it.table[i].iddatatype)
				{
				case IT::IDDATATYPE::INT:
					*out << std::setw(11) << "short | " << it.table[i].value.vint << std::endl;
					break;
				case IT::IDDATATYPE::STR:
					*out << std::setw(11) << "string | " << it.table[i].value.vstr.str << std::endl;
					break;
				case IT::IDDATATYPE::BL:
					*out << std::setw(11) << "bool | ";
					if (it.table[i].value.vbool)*out << "true" << std::endl;
					else *out << "false" << std::endl;;
					break;
				case IT::IDDATATYPE::CHR:
					*out << std::setw(11) << "char | " << it.table[i].value.vchar << std::endl;
					break;
				default:
					*out << std::setw(11) << "void | " << "none" << std::endl;
					break;
				}

			}
		}
	}

	void FullLexTable(std::ofstream* out, LT::LexTable lt, IT::IdTable it)
	{
		int number = 0;
		*out << std::setfill('0') << std::setw(5) << number << " " << std::setfill(' ');
		for (int i = 0; i < lt.size; ++i)
		{
			if (lt.table[i].lexema != NULL)
			{
				if (number != lt.table[i].sn)
				{
					*out << std::endl << std::setfill('0') << std::setw(5) << number << " " << std::setfill(' ');
					number = lt.table[i].sn;
				}
				*out << lt.table[i].lexema[0];
				if (lt.table[i].lexema[0] == SEQ)*out << it.table[lt.table[i].idxTI].value.vint;
				else if (lt.table[i].lexema[0] == LEX_OPERATION)*out << '{' << it.table[lt.table[i].idxTI].id << '}';
			}
		}
		*out << std::endl;
	}
}