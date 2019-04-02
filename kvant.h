typedef unsigned short int HALFWORD;
typedef unsigned char BYTE;
typedef HALFWORD MZU;
typedef struct {
	BYTE kop, nor, uta, smeshenie;
	BYTE nrba, rp, nir, uti, kor, ung;
	HALFWORD *ron;
	BYTE level;
	HALFWORD *ip;
} CUU;

/*

              1-format (0 halfword)
 16                                          0
+-------+-------+-----+-----------------------+
|  KOP  |  NOR  | UTA |       Smeshenie       |
+-------+-------+-----+-----------------------+
    3       3      2              8


              2-format (1 halfword)
 16                                          0
+-------+-----+-----+-----+---------+---------+
|  NRBA | R/P | NIR | UTI |   KOR   |   UNG   |
+-------+-----+-----+-----+---------+---------+
    3      1     2     2       4         4

*/

#define CUU_KOP_KON 0b000	// AND
#define CUU_KOP_ASL 0b001	// ADD
#define CUU_KOP_PRS 0b010	// MOV
#define CUU_KOP_NEK 0b011	// XOR
#define CUU_KOP_DIZ 0b100	// OR

#define CUU_UTA_ROA 0b00
#define CUU_UTA_SOR 0b01
#define CUU_UTA_SOP 0b10
#define CUU_UTA_PSR 0b11

#define CUU_UTI_OBI 0b00
#define CUU_UTI_OPI 0b01
#define CUU_UTI_OSI 0b10
#define CUU_UTI_OVI 0b11

#define MZU_FIRST_PAGE 0x20000
#define MZU_LEVEL_SIZE 0x20
#define MZU_SIZE (MZU_LEVEL_SIZE * 4 + 0x1ff80 + 8)

extern void conv_to_olmp(HALFWORD halfword, BYTE *olmp);

extern void cuu_dump(CUU **cuu, MZU **mzu);
extern void cuu_init(CUU **cuu, MZU **mzu);
extern void cuu_free(CUU **cuu, MZU **mzu);
extern void cuu_exec(CUU **cuu, MZU **mzu);
