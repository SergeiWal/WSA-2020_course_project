#include "Error.h"

namespace Error
{
	ERROR errors[ERROR_MAX_ENTRY] =
	{
		ERROR_ENTRY(0, "������������ ��� ������"),
		ERROR_ENTRY(1, "��������� ����"),
		ERROR_ENTRY_NODEF(2), ERROR_ENTRY_NODEF(3), ERROR_ENTRY_NODEF(5), ERROR_ENTRY_NODEF(6),
		ERROR_ENTRY_NODEF(6), ERROR_ENTRY_NODEF(7), ERROR_ENTRY_NODEF(8), ERROR_ENTRY_NODEF(9),
		ERROR_ENTRY_NODEF10(10), ERROR_ENTRY_NODEF10(20), ERROR_ENTRY_NODEF10(30), ERROR_ENTRY_NODEF10(40), ERROR_ENTRY_NODEF10(50),
		ERROR_ENTRY_NODEF10(60),ERROR_ENTRY_NODEF10(70),ERROR_ENTRY_NODEF10(80),ERROR_ENTRY_NODEF10(90),
		ERROR_ENTRY(100, "�������� -in ������ ���� �����"),
		ERROR_ENTRY_NODEF(101), ERROR_ENTRY_NODEF(102), ERROR_ENTRY_NODEF(103),
		ERROR_ENTRY(104, "��������� ����� �������� ���������"),
		ERROR_ENTRY_NODEF(105),ERROR_ENTRY_NODEF(106),ERROR_ENTRY_NODEF(107),
		ERROR_ENTRY_NODEF(108),ERROR_ENTRY_NODEF(109),
		ERROR_ENTRY(110,"������ ��� �������� ����� � �������� ����� (-in)"),
		ERROR_ENTRY(111, "������������ ������ � �������� ����� (-in)"),
		ERROR_ENTRY(112, "������ ��� �������� ����� ��������� (-log)"),
		ERROR_ENTRY_NODEF(113), ERROR_ENTRY_NODEF(114), ERROR_ENTRY_NODEF(115), ERROR_ENTRY_NODEF(116),
		ERROR_ENTRY(117, "��������� ������������ ������� ������� ������"),
		ERROR_ENTRY(118, "������� ������ ������� ������ �������� ������������ ��������"),
		ERROR_ENTRY(119, "������ ����������� � ������� ������"),
		ERROR_ENTRY(120, "��������� ������������ ������� ������� ��������������"),
		ERROR_ENTRY(121, "������� ������ ������� �������������� �������� ������������ ��������"),
		ERROR_ENTRY(122, "������ ����������� � ������� ��������������"),
		ERROR_ENTRY(123, "�� ������� ��������� �������"),
		ERROR_ENTRY(124, "�� ������� ������� ����"),
		ERROR_ENTRY(125, "������������� �������������"),
		ERROR_ENTRY(126, "����������� ��� ������ ��������������"),
		ERROR_ENTRY(127, "��������� ���������� ��������������"),
		ERROR_ENTRY(128, "������� ������������ �������� ������ �����"),
		ERROR_ENTRY(129, "������� ����������� �������� ������ �����"),
		ERROR_ENTRY(130, "�������� ������������ ������ ������"),
		ERROR_ENTRY(131, "����������� ����� ����� � ���������"),
		ERROR_ENTRY(132, "���������� ��������� ����� ����� � ���������"),
		ERROR_ENTRY_NODEF(133),	ERROR_ENTRY_NODEF(134),	ERROR_ENTRY_NODEF(135),
		ERROR_ENTRY_NODEF(136), ERROR_ENTRY_NODEF(137),	ERROR_ENTRY_NODEF(138), ERROR_ENTRY_NODEF(139), 
		ERROR_ENTRY_NODEF10(140), ERROR_ENTRY_NODEF10(150), ERROR_ENTRY_NODEF10(160),
		ERROR_ENTRY_NODEF10(170),ERROR_ENTRY_NODEF10(180),ERROR_ENTRY_NODEF10(190),
		ERROR_ENTRY_NODEF100(200), 
		ERROR_ENTRY(300, "�������������� ����� ���������� �������(���������)"),
		ERROR_ENTRY(301, "�������������� ���������� ��������� �������(���������)"),
		ERROR_ENTRY(302, "����������� ����������� }"),
		ERROR_ENTRY(303, "������� ������� �� �������"),
		ERROR_ENTRY(304, "�������������� ����� ��������� ���������"),
		ERROR_ENTRY(305, "�������������� ���� ������������� �������� ������� �������"),
		ERROR_ENTRY(306, "�������� ��������� � ���������"),
		ERROR_ENTRY(307, "����������� ����������� {"),
		ERROR_ENTRY(308, "�������� ���������� � ������� ����������"),
		ERROR_ENTRY(309, "���������� ������ ���� ��������� ��� ���������"),	
		ERROR_ENTRY(310, "�������������� ������������� �������� �������"),
		ERROR_ENTRY_NODEF(311),	ERROR_ENTRY_NODEF(312),	ERROR_ENTRY_NODEF(313),
		ERROR_ENTRY_NODEF(314), ERROR_ENTRY_NODEF(315),	ERROR_ENTRY_NODEF(316), ERROR_ENTRY_NODEF(317),
		ERROR_ENTRY_NODEF(318),	ERROR_ENTRY_NODEF(319),
		ERROR_ENTRY_NODEF10(320), ERROR_ENTRY_NODEF10(330),
		ERROR_ENTRY_NODEF10(340),ERROR_ENTRY_NODEF10(350),ERROR_ENTRY_NODEF10(360),
		ERROR_ENTRY_NODEF10(370), ERROR_ENTRY_NODEF10(380), ERROR_ENTRY_NODEF10(390),
		ERROR_ENTRY_NODEF100(400), ERROR_ENTRY_NODEF100(500),
		ERROR_ENTRY(600, "�������������� ������: �������� ��������� ���������"),
		ERROR_ENTRY(601, "�������������� ������: �������� ���������� �����"),
		ERROR_ENTRY(602, "�������������� ������: ������ � ���������"),
		ERROR_ENTRY(603, "�������������� ������: ��������� ��������"),
		ERROR_ENTRY(604, "�������������� ������: ������ � ��������� �������"),
		ERROR_ENTRY(605, "�������������� ������: ������ � ���������� ���������� �������"),
		ERROR_ENTRY(606, "�������������� ������: ���������� ������ �� ����� ��������������� �������"),
		ERROR_ENTRY_NODEF(607), ERROR_ENTRY_NODEF(608), ERROR_ENTRY_NODEF(609),
		ERROR_ENTRY_NODEF10(610), ERROR_ENTRY_NODEF10(620), ERROR_ENTRY_NODEF10(630), ERROR_ENTRY_NODEF10(640),
		ERROR_ENTRY_NODEF10(650),ERROR_ENTRY_NODEF10(660),ERROR_ENTRY_NODEF10(670),ERROR_ENTRY_NODEF10(680),
		ERROR_ENTRY_NODEF10(690),
		ERROR_ENTRY_NODEF100(700), ERROR_ENTRY_NODEF100(800), ERROR_ENTRY_NODEF100(900)
	};

	ERROR geterror(int id)
	{
		ERROR error;
		if (id <= 0 || id > ERROR_MAX_ENTRY)
		{
			error.id = errors[0].id;
			sprintf_s(error.message, "%s", errors[0].message);
			return error;
		}
		else
		{
			error.id = errors[id].id;
			sprintf_s(error.message, "%s", errors[id].message);
			return error;
		}
	}

	ERROR geterrorin(int id, int line = -1, int col = -1) {
		ERROR error;
		if (id <= 0 || id > ERROR_MAX_ENTRY)
		{
			error.id = errors[0].id;
			sprintf_s(error.message, "%s", errors[0].message);
			error.inext.col = col;
			error.inext.line = line;
			return error;
		}
		else 
		{
			error.id = errors[id].id;
			sprintf_s(error.message, "%s", errors[id].message);
			error.inext.col = col;
			error.inext.line = line;
			return error;
		}
		
	}
};