#include "CG.h"

namespace CG
{
	std::ofstream* CreateAsmFile()
	{
		std::ofstream* stream = new std::ofstream;
		stream->open(FILENAME);
		if (!stream->is_open())throw ERROR_THROW(124);
		*stream << PROC_SERIES << ENDL << MODEL << ENDL << LIBRARY
			<< ENDL << EXIT_PROC << ENDL << STACK << ENDL;
		return stream;
	}

	void ConstBlockFill(std::ofstream* file, IT::IdTable it)
	{
		*file << CONST << ENDL;
		for (int i = 0; i < it.size; ++i)
		{
			if (it.table[i].idtype == IT::IDTYPE::L && it.table[i].id != NULL)
			{
				*file << it.table[i].visibilityRegion << '_' << it.table[i].id << SPACE;
				switch (it.table[i].iddatatype)
				{
				case IT::IDDATATYPE::BL:
					*file << BYTE << SPACE;
					if (it.table[i].value.vbool)*file << TRUE;
					else *file << FALSE;
					break;
				case IT::IDDATATYPE::INT:
					*file << SHORT << SPACE
						<< it.table[i].value.vint;
					break;
				case IT::IDDATATYPE::STR:
					*file << BYTE << SPACE << '\"' << it.table[i].value.vstr.str << '\"' << LEX_COMA << 0;
					break;
				default:
					break;
				}
				*file << ENDL << ENDL;
			}
		}
	}

	void SetDataValue(std::ofstream* file, IT::IdTable it, int pos)
	{
		switch (it.table[pos].iddatatype)
		{
		case IT::IDDATATYPE::BL:
			*file << BYTE << SPACE;
			if (it.table[pos].value.vbool)*file << TRUE;
			else *file << FALSE;
			break;
		case IT::IDDATATYPE::INT:
			*file << SHORT << SPACE
				<< it.table[pos].value.vint;
			break;
		case IT::IDDATATYPE::STR:
			*file << BYTE << SPACE << "0h" << ENDL; //!
			*file << SPACE << SPACE << BYTE << SPACE << TI_STR_MAXSIZE << "dup(0h)";//!
			break;
		default:
			break;
		}
	}

	void DataBLockFILL(std::ofstream* file, IT::IdTable it)
	{
		*file << DATA << ENDL;
		for (int i = 0; i < it.size; ++i)
		{
			switch (it.table[i].idtype)
			{
			case IT::IDTYPE::V:

				*file << ";------variable-------------" << it.table[i].id << "--------------variable--------" << ENDL;
				*file << it.table[i].visibilityRegion << '_' << it.table[i].id << SPACE;
				SetDataValue(file, it, i);
				*file << ENDL;
				break;
			case IT::IDTYPE::F:
				*file << ";-------------------" << it.table[i].id << " return value -----------------" << ENDL;
				*file << RET << it.table[i].id << SPACE;
				SetDataValue(file, it, i);
				*file << ENDL;
				break;
			case IT::IDTYPE::A:
				*file << ";-------------------" << it.table[i].visibilityRegion << " parameters data -----------------" << ENDL;
				if (it.table[i].visibilityRegion == MAIN_VISIBLE)*file << it.table[i].id << SPACE;
				else *file <<
					it.table[i].visibilityRegion << '_' << it.table[i].id << SPACE;
				SetDataValue(file, it, i);
				*file << ENDL;
				break;
			default:
				break;
			}
		}
	}

	void generationOperation(std::ofstream* file, char opr, IT::IDDATATYPE type, int number, bool isBinary)
	{
		switch (opr)
		{
		case '+':
			if (type == IT::IDDATATYPE::INT)*file << "pop ebx" << ENDL << "pop eax" << ENDL
				<< "add eax,ebx" << ENDL << "push eax" << ENDL;
			else if (type == IT::IDDATATYPE::STR)*file << "call concat" << ENDL;
			break;
		case '-':
			if (isBinary)*file << "pop ebx" << ENDL << "pop eax" << ENDL
				<< "sub eax,ebx" << ENDL << "push eax" << ENDL;
			else *file << "pop eax" << ENDL << "neg eax" << ENDL;
			break;
		case '*':
			*file << "pop ebx" << ENDL << "pop eax" << ENDL
				<< "imul ebx" << ENDL << "push eax" << ENDL;
			break;
		case '/':
			*file << "pop ebx" << ENDL << "pop eax" << ENDL
				<< "idiv ebx" << ENDL << "push eax" << ENDL;
			break;
		case '%':
			*file << "pop ebx" << ENDL << "pop eax" << ENDL
				<< "idiv ebx" << ENDL << "push edx" << ENDL;
			break;
		case '<':
			if (type == IT::IDDATATYPE::STR)*file << "pop ebx" << ENDL << "pop eax" << ENDL
				<< "move eax, lengtof eax" << ENDL << "mov ebx, lengtof ebx" << ENDL
				<< "cmp eax,ebx" << ENDL << "js " << "true_" << number << ENDL
				<< "push 0" << ENDL << "jmp " << "end_" << number << ENDL
				<< "true_" << number << ": " << ENDL << "\t push 1" << ENDL
				<< "end_" << number << ": " << ENDL;
			else *file << "pop ebx" << ENDL << "pop eax" << ENDL
				<< "cmp eax,ebx" << ENDL << "js " << "true_" << number << ENDL
				<< "push 0" << ENDL << "jmp " << "end_" << number << ENDL
				<< "true_" << number << ": " << ENDL << "\t push 1" << ENDL
				<< "end_" << number << ": " << ENDL;
			break;
		case '>':
			if (type == IT::IDDATATYPE::STR)*file << "pop ebx" << ENDL << "pop eax" << ENDL
				<< "move eax, lengtof eax" << ENDL << "mov ebx, lengtof ebx" << ENDL
				<< "cmp eax,ebx" << ENDL << "jns " << "true_" << number << ENDL
				<< "push 0" << ENDL << "jmp " << "end_" << number << ENDL
				<< "true_" << number << ": " << ENDL << "\t push 1" << ENDL
				<< "end_" << number << ": " << ENDL;
			else *file << "pop ebx" << ENDL << "pop eax" << ENDL
				<< "cmp eax,ebx" << ENDL << "jns " << "true_" << number << ENDL
				<< "push 0" << ENDL << "jmp " << "end_" << number << ENDL
				<< "true_" << number << ": " << ENDL << "\t push 1" << ENDL
				<< "end_" << number << ": " << ENDL;
			break;
		case '=':
			if (type == IT::IDDATATYPE::STR)*file << "pop ebx" << ENDL << "pop eax" << ENDL
				<< "move eax, lengtof eax" << ENDL << "mov ebx, lengtof ebx" << ENDL
				<< "cmp eax,ebx" << ENDL << "jz " << "true_" << number << ENDL
				<< "push 0" << ENDL << "jmp " << "end_" << number << ENDL
				<< "true_" << number << ": " << ENDL << "\t push 1" << ENDL
				<< "end_" << number << ": " << ENDL;
			else *file << "pop ebx" << ENDL << "pop eax" << ENDL
				<< "cmp eax,ebx" << ENDL << "jz " << "true_" << number << ENDL
				<< "push 0" << ENDL << "jmp " << "end_" << number << ENDL
				<< "true_" << number << ": " << ENDL << "\t push 1" << ENDL
				<< "end_" << number << ": " << ENDL;
			break;
		case '!':
			if (type == IT::IDDATATYPE::STR)*file << "pop ebx" << ENDL << "pop eax" << ENDL
				<< "move eax, lengtof eax" << ENDL << "mov ebx, lengtof ebx" << ENDL 
				<< "cmp eax,ebx" << ENDL << "jnz " << "true_" << number << ENDL
				<< "push 0" << ENDL << "jmp " << "end_" << number << ENDL
				<< "true_" << number << ": " << ENDL << "\t push 1" << ENDL
				<< "end_" << number << ": " << ENDL;
			else *file << "pop ebx" << ENDL << "pop eax" << ENDL
				<< "cmp eax,ebx" << ENDL << "jnz " << "true_" << number << ENDL
				<< "push 0" << ENDL << "jmp " << "end_" << number << ENDL
				<< "true_" << number << ": " << ENDL << "\t push 1" << ENDL
				<< "end_" << number << ": " << ENDL;
			break;
		default:
			break;
		}
	}

	void CodeBlockFILL(std::ofstream* file, IT::IdTable it, LT::LexTable lt)
	{
		bool isMain = false;
		bool isCycle = false;
		bool isWrite = false;
		bool isReturn = false;
		bool isBinary = true;
		int resultBufferIdx = -1;		//������ ���������� ����������, ���� -1 �� �� �������
		int parameterCount;				//���-�� ���������� ������ �������
		std::string currentCycleName;   //��� ����� - ������������ ��� �������� �����
		std::string funcName;			//��� ������� �������		
		IT::IDDATATYPE currentType = IT::IDDATATYPE::NONE;			//��� ������  ���������� ��������������

		*file << CODE_BLOCK << ENDL;

		for (int i = 0;i<lt.size;++i)
		{
			switch (lt.table[i].lexema[0])
			{
			case LEX_ID:
				currentType = it.table[lt.table[i].idxTI].iddatatype;
				if (lt.table[i + 1].lexema[0] == LEX_EQUALL)
				{
					resultBufferIdx = i;
					i++;
				}
				else
				{
					*file << "push ";
					if (it.table[lt.table[i].idxTI].iddatatype == IT::IDDATATYPE::STR)*file << "offset ";
					*file << it.table[lt.table[i].idxTI].visibilityRegion
						<< '_' << it.table[lt.table[i].idxTI].id << ENDL;
				}
				break;
			case LEX_LITERAL:
				*file << "push ";
				if (it.table[lt.table[i].idxTI].iddatatype == IT::IDDATATYPE::STR)*file << "offset ";
				*file << it.table[lt.table[i].idxTI].visibilityRegion
					<< '_' << it.table[lt.table[i].idxTI].id << ENDL;
				break;
			case LEX_CALL:
				while (lt.table[i].lexema[0] != SEQ)i++;
				parameterCount = it.table[lt.table[i].idxTI].value.vint;
				for (int j = parameterCount; j > 0; --j)
				{
					--i;
					*file << "push " << it.table[lt.table[i].idxTI].visibilityRegion
						<< '_' << it.table[lt.table[i].idxTI].id << ENDL;
				}
				--i;
				*file << "call " << it.table[lt.table[i].idxTI].id << ENDL;
				i += parameterCount + 1;
				break;
			case LEX_OPERATION:
				if (lt.table[i - 2].lexema[0] == LEX_EQUALL)isBinary = false;
				generationOperation(file, it.table[lt.table[i].idxTI].id[0], currentType, i, isBinary);
				isBinary = true;
				break;
			case LEX_FUNCTION:
				i++;
			case LEX_PROC:
				i++;
				*file << (funcName = it.table[lt.table[i].idxTI].id) << " PROC " << SAVE_REGISTRS << ENDL;
				while (lt.table[i].lexema[0] != LEX_RIGHTHESIS)++i;
				parameterCount = i;//������� �� �� ����������
				while (lt.table[i].lexema[0] != LEX_LEFTHESIS)
				{
					if (lt.table[i].lexema[0] == LEX_ID)*file << "pop "
						<< it.table[lt.table[i].idxTI].visibilityRegion
						<< '_' << it.table[lt.table[i].idxTI].id << ENDL;
					--i;
				}
				i = parameterCount;
				break;
			case LEX_RETURN:
				isReturn = true;
				break;
			case LEX_SEMICOLON:
				if (resultBufferIdx != -1)
				{
					*file << "pop " << it.table[lt.table[resultBufferIdx].idxTI].visibilityRegion
						<< '_' << it.table[lt.table[resultBufferIdx].idxTI].id << ENDL;
					resultBufferIdx = -1;
				}
				else if (isReturn)
				{
					*file << "pop eax" << ENDL << "mov " << RET << funcName << LEX_COMA << "eax" << ENDL
						<< "ret" << ENDL;
					isReturn = false;
				}
				break;
			case LEX_MAIN:
				*file << "SWA2020 PROC " << SAVE_REGISTRS << ENDL;
				funcName = "SWA2020";
				isMain = true;
				break;
			case LEX_LEFTBRACE:
				if (isCycle)
				{
					*file << "pop eax" << ENDL
						<< "cmp eax,1" << ENDL << "jz " << currentCycleName << "_BEGIN" << ENDL
						<< "jnz " << currentCycleName << "_END" << ENDL;
				}
				break;
			case LEX_BRACELET:
				if (!isCycle)
				{
					if (isMain)
					{
						isMain = false;
						*file << "push 0h" << ENDL;
					}
					*file << funcName << " ENDP" << ENDL;
				}
				else
				{
					*file << "jmp " << currentCycleName << ENDL
						<< currentCycleName << "_END: " << ENDL;
					isCycle = false;
				}
				break;
			case LEX_CYCLE:
				isCycle = true;
				currentCycleName = "CYCLE_" + std::to_string(i);
				*file << currentCycleName << ": " << ENDL;
				break;
			case LEX_VAR:
				i += 2;
				break;
			case LEX_WRITE:
				while (lt.table[i + 1].lexema[0] != LEX_SEMICOLON)i++;
				break;
			default:
				break;
			}
		}
	}

	void asmMain(std::ofstream* file)
	{
		*file << "main PROC" << ENDL << ENDL << "call WSA2020" << ENDL
			<< "push eax" << ENDL << "call ExitProcess" << ENDL
			<< "main ENDP" << ENDL << "end main";
	}

	void CloseFile(std::ofstream* file)
	{
		file->close();
		delete[] file;
	}
};
