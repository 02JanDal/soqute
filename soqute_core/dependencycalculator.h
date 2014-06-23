#pragma once

#include <QObject>

#include "soqute_core_global.h"

class Package;
class PackageList;

typedef const Package *PackagePointer;
typedef QList<PackagePointer> PackagePointerList;

class SOQUTE_CORESHARED_EXPORT DependencyCalculator : public QObject
{
	Q_OBJECT
public:
	explicit DependencyCalculator(PackageList *packageList, PackagePointerList packages,
								  QObject *parent = 0);

	enum Direction {
		Forward,
		Reverse
	};

	/**
	 * \returns The result of the dependency calculation
	 */
	PackagePointerList result() const
	{
		return m_result;
	}

public
slots:
	/**
	 * This function will calculate the dependencies of all packages specified in the
	 * constructor recursivly,
	 * and add them to the result which can be accesses using \see result
	 */
	bool calculateDependencies(const Direction direction = Forward,
							   const Package **notFoundPackage = 0);

	void setStartingPoint(const PackagePointerList packages);

signals:
	void noSuchPackage(const QString &id, const QString &version, const QString &host, const QString &target);
	void done();

private:
	/// The calculated dependencies, including the dependents
	PackagePointerList m_result;
	/// The packages to calculate dependencies for
	PackagePointerList m_startingPoint;

	/// A pointer to the list of packages
	PackageList *m_packages;

	PackagePointerList calculateDependenciesForward(const Package *package, bool *ok = 0,
													const Package **notFoundPackage = 0);
	PackagePointerList calculateDependenciesReverse(const Package *package,
													const QString &intendent = "");
};
