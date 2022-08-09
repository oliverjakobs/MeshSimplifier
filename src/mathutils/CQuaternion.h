#ifndef CQUATERNION_H_INCLUDED
#define CQUATERNION_H_INCLUDED

// includes
#include "CMatrix.h"

class CQuaternion
{
private:
	double value_[4];

public:
	// Standard-Konstruktor
	CQuaternion(
		);

	// Copy Constructor
	CQuaternion(
		const CQuaternion& quat_in
		);

	// Konstruktor mit direkter Initialisierung der Komponenten
	CQuaternion(
		const double& a_in,
		const double& b_in,
		const double& c_in,
		const double& d_in
		);

	// Konstruktor des Einheitsquaternions über Achse und Winkel
	CQuaternion(
		const CVector &vecAxis_in, 
		const double &dAngle_in
		);

	// Zuweisungs-Operator
	CQuaternion&
	operator=(
		const CQuaternion& quat_in
		);


	// [] Operator für direkten Zugriff auf die Komponenten
	// Mit diesem Operator kann ich auf die einzelnen Vektorkomponenten lesend und schreibend zugreifen!
	double&
	operator[](
		const unsigned short index_in
		);

	// [] Operator für direkten Zugriff auf die Komponenten (const Version)
	// (Mit diesem Operator kann ich bei konstanten Vektoren auf die Vektorkomponenten lesend zugreifen!)
	double
	operator[](
		const unsigned short index_in
		) const;


	// Setze alle Komponenten 
	void
	set(
		const double& a_in, 
		const double& b_in,
		const double& c_in,
		const double& d_in
		);

	// Lese alle Komponenten 
	void
	get(
		double& a_out, 
		double& b_out,
		double& c_out,
		double& d_out
		) const;

	// Lese Achse und Winkel der Rotation
	void
	getRotation(
		 CVector &axis, 
		 double &dAngle
		) const;

	// Setze Achse und Winkel der Rotation
	void 
	setRotation(
		const CVector &vecAxis_in, 
		const double &dAngle_in
		);

	// Setze Quaternion auf die Rotation aus der
	// gegebenen Matrix
	CQuaternion &
	setMatrix4x4(const CMatrix &m);

	// Vergleichs-Operator
	bool
	operator==(
		const CQuaternion& vector_in
		) const;

	// negativer Vergleichs-Operator
	bool
	operator!=(
		const CQuaternion& vector_in
		) const;

	// negatives Quaternion (unärer Minus-Operator)
	CQuaternion
	operator-(
		) const;


	// Subtraktions-Operator für die Quaternionen-Differenz (binärer Minus-Operator)
	friend   CQuaternion
	operator-(
		const CQuaternion& vector0_in,
		const CQuaternion& vector1_in
		);

	// Additions-Operator für die Quaternionen-Summe (binärer Minus-Operator)
	friend   CQuaternion
	operator+(
		const CQuaternion& vector0_in,
		const CQuaternion& vector1_in
		);

	//  Multiplikation Quaternion mit Quaternion
	friend   CQuaternion
	operator*(
		const CQuaternion& vector0_in,
		const CQuaternion& vector1_in
		);


	//  Multiplikation Skalar mit Quaternion
	friend   CQuaternion
	operator*(
		const double& scalar_in,
		const CQuaternion& quat_in
		);

	//  Multiplikation Quaternion mit Skalar
	friend   CQuaternion
	operator*(
		const CQuaternion& quat_in,
		const double& scalar_in
		);

	// Division Quaternion durch Skalar
	friend   CQuaternion
	operator/(
		const CQuaternion& quat_in,
		const double& scalar_in
		);

	// Skalarprodukt zweier Quaternionen
	friend   double
	dot(
		const CQuaternion& quat0_in,
		const CQuaternion& quat1_in
		);

	// Konjugiert komplexer Quaternion
	friend   CQuaternion
	conjugate(
		const CQuaternion& quat_in
		);
	
	// Sphärisch Lineare Interpolation zwischen zwei Quaternionen mit 
	// Interpolationsgewicht <weight_in>
	friend   CQuaternion
	slerp(
		double weight_in,
		const CQuaternion& quat0_in,
		const CQuaternion& quat1_in
		);

	// Increment-Operator
	CQuaternion&
	operator+=(
		const CQuaternion& vector_in
		);

	// Dekrement-Operator
	CQuaternion&
	operator-=(
		const CQuaternion& vector_in
		);

	// Skalierungs-Operator *= 
	CQuaternion&
	operator*=(
		const double& scalar_in
		);

	// Divisions-Operator /=
	CQuaternion&
	operator/=(
		const double& scalar_in
		);

	// Länge des Quaternions
	double
	getLength(
		) const;

	// bestimme den Einheits-Quaternion (mit Out-Argument)
	void
	getNormalized(
		CQuaternion& vector_out
		) const;

	// bestimme den Einheits-Quaternion (mit Rückgabewert)
	CQuaternion
	getNormalized(
		) const;

	// normiere den Quaternion auf Einheitslänge
	void
	normalize(
		);

	// Abfrage, ob Einheitsquaternion
	bool
	isNormalized(
		) const;

	// Abfrage, ob Null-Quaternion
	bool
	isNil(
		) const;

	// Konvertiereung Quaternion in Rotationsmatrix
	CMatrix 
	getMatrix4x4(
	    ) const;

};

#endif // CQUATERNION_H_INCLUDED
