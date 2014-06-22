#include "updatecommand.h"

#include <QCommandLineParser>

#include "loadmetadata.h"
#include "textstream.h"

UpdateCommand::UpdateCommand(LoadMetadata *metadataLoader, ConfigurationHandler *configHandler, PackageList *packages, QObject *parent)
	: BaseCommand(configHandler, packages, parent), m_metadataLoader(metadataLoader) {}

void UpdateCommand::setupParser()
{
	parser = new QCommandLineParser();
	parser->setApplicationDescription(tr("Forces a re-download of all repositories"));
	parser->addHelpOption();
	parser->addVersionOption();
}

bool UpdateCommand::executeImplementation()
{
	auto checkAndOutputMessages = [this]()
	{
		while (m_metadataLoader->hasMessage()) {
			QPair<LoadMetadata::Message, QVariant> msg = m_metadataLoader->takeLastMessage();
			out << LoadMetadata::messageToString(msg) << endl << flush;
		}
	};

	m_metadataLoader->loadMetadata(true);
	while (!m_metadataLoader->isDone()) {
		qApp->processEvents();
		checkAndOutputMessages();
	}
	checkAndOutputMessages();
	if (m_metadataLoader->isSuccess()) {
		out << "Done." << endl;
		return true;
	} else {
		out << "Failure." << endl;
		return false;
	}
}
