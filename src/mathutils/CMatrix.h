#ifndef CMATRIX_H_INCLUDED
#define CMATRIX_H_INCLUDED


class CVector;

class    CMatrix
{
protected:
	double value_[4][4];	// [row][column]
	// (0,0) (0,1) (0,2) (0,3)
	// (1,0) (1,1) (1,2) (1,3)
	// (2,0) (2,1) (2,2) (2,3)
	// (3,0) (3,1) (3,2) (3,3)

public:
	// Standard-Konstruktor
	CMatrix(
		);

	// Copy-Konstruktor
	CMatrix(
		const CMatrix& matrix_in
		);

	// Konstruktor mit Initialisierung
	CMatrix(
		const double& a00_in,
		const double& a01_in,
		const double& a02_in,
		const double& a03_in,
		const double& a10_in,
		const double& a11_in,
		const double& a12_in,
		const double& a13_in,
		const double& a20_in,
		const double& a21_in,
		const double& a22_in,
		const double& a23_in,
		const double& a30_in,
		const double& a31_in,
		const double& a32_in,
		const double& a33_in
		);

	// Konstruktor mit Initialisierung über Array
	CMatrix(
		const double a_in[16]
		);

	// Zuweisungs-Operator
	CMatrix&
	operator=(
		const CMatrix& matrix_in
		);

	// Zeilenindex-Operator
	CVector
	operator[](
		const unsigned short rowIndex_in
		) const;

	// Element-Index Operator runde Klammern
	// für Lese/Schreibzugriff
	double&
	operator()(
		const unsigned short rowIndex_in,
		const unsigned short colIndex_in
		);

	// Element-Index Operator runde Klammern
	// const-Version nur für Lesezugriff
	double
	operator()(
		const unsigned short rowIndex_in,
		const unsigned short colIndex_in
		) const;

	// Setze alle Elemente
	void
	set(
		const double& a00_in,
		const double& a01_in,
		const double& a02_in,
		const double& a03_in,
		const double& a10_in,
		const double& a11_in,
		const double& a12_in,
		const double& a13_in,
		const double& a20_in,
		const double& a21_in,
		const double& a22_in,
		const double& a23_in,
		const double& a30_in,
		const double& a31_in,
		const double& a32_in,
		const double& a33_in
		);

	// Lese alle Elemente
	void
	get(
		double& a00_out,
		double& a01_out,
		double& a02_out,
		double& a03_out,
		double& a10_out,
		double& a11_out,
		double& a12_out,
		double& a13_out,
		double& a20_out,
		double& a21_out,
		double& a22_out,
		double& a23_out,
		double& a30_out,
		double& a31_out,
		double& a32_out,
		double& a33_out
		) const;

	// Setze Matrix auf Nullmatrix
	void
	setZeroMatrix(
		);

	// generiere eine Nullmatrix
	static CMatrix
	zeroMatrix(
		);

	// Setze Matrix auf Identitätsmatrix (Einheitsmatrix)
	void
	setIdentityMatrix(
		);

	// generiere eine Identitätsmatrix (Einheitsmatrix)
	static CMatrix
	identityMatrix(
		);

	// Vergleichsoperator (Koeffizienten-Vergleich)
	bool
	operator==(
		const CMatrix& matrix_in
		) const;

	// negierter Vergeichsoperator (Koeffizienten-Vergleich)
	bool
	operator!=(
		const CMatrix& matrix_in
		) const;

	// Negierung (unäres Minus)
	CMatrix
	operator-(
		) const;

	// Matrix-Differenz-Operator (binäres Minus)
	friend   CMatrix
	operator-(
		const CMatrix& matrix0_in,
		const CMatrix& matrix1_in
		);

	// Matrix-Additions-Operator (binäres Plus)
	friend   CMatrix
	operator+(
		const CMatrix& matrix0_in,
		const CMatrix& matrix1_in
		);

	// Matrix-Multiplikation
	friend   CMatrix
	operator*(
		const CMatrix& matrix0_in,
		const CMatrix& matrix1_in
		);

	// Multiplikation Skalar-Matrix
	friend   CMatrix
	operator*(
		const double& scalar_in,
		const CMatrix& matrix_in
		);

	// Multiplikation Matrix-Skalar
	friend   CMatrix
	operator*(
		const CMatrix& matrix_in,
		const double& scalar_in
		);

	// Multiplikation Zeilenvektor-Matrix
	friend   CVector
	operator*(
		const CVector& rowVector_in,
		const CMatrix& matrix_in
		);

	// Multiplikation Spaltenvektor-Matrix
	friend   CVector
	operator*(
		const CMatrix& matrix_in,
		const CVector& colVector_in
		);

	// Matrix-Inkrement-Operator
	CMatrix&
	operator+=(
		const CMatrix& matrix_in
		);

	// Matrix-Dekrement-Operator
	CMatrix&
	operator-=(
		const CMatrix& matrix_in
		);

	// Matrix-Matrix-Multiplikations-Operator *=
	CMatrix&
	operator*=(
		const CMatrix& matrix_in
		);

	// Matrix Skalierungs-Operator *=
	CMatrix&
	operator*=(
		const double& scalar_in
		);

	// Matrix-Invertierung (ersetzt die Matrix durch ihre Inverse)
	void
	invert(
		);

	// Matrix-Invertierung (läßt die Matrix unverändert und gibt die Inverse zurück)
	CMatrix
	getInverse(
		) const;

	// Transponierung(ersetzt die Matrix durch ihre Transponierte)
	void
	transpose(
		);

	// Matrix-Transponierung (läßt die Matrix unverändert und gibt die Transponierte zurück)
	CMatrix
	getTransposed(
		) const;

	// Abfrage, ob Identitätsmatrix vorliegt
	bool
	isIdentityMatrix(
		) const;

	// Abfrage, ob eine Nullmatrix vorliegt
	bool
	isZeroMatrix(
		) const;

	// bestimme die Unterdeterminante zum Element <rowIndex,colIndex>
	double 
	getSubDeterminant(
		const unsigned short rowIndex_in,
		const unsigned short colIndex_in
		) const;

	// bestimme die Determinante der Matrix
	double
	getDeterminant(
		) const;

	// bestimme die Euler-Winkel der Matrix
	CVector
	getEulerAngles(
		) const;

	// bestimme den Translationsvektor
	CVector
	getTranslation(
		) const;
};

#endif // CMATRIX_H_INCLUDED
