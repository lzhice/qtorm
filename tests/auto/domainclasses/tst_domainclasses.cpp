/*
 * Copyright (C) 2019-2021 Dmitriy Purgin <dmitriy.purgin@sequality.at>
 * Copyright (C) 2019 sequality software engineering e.U. <office@sequality.at>
 *
 * This file is part of QtOrm library.
 *
 * QtOrm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * QtOrm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with QtOrm.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <QFile>
#include <QtTest>

#include <QOrmSession>

class DomainClassesTest : public QObject
{
    Q_OBJECT

private slots:
    void init();

    void testQVectorReference();
    void testQSetReference();
    void testStdVectorReference();
    void testNullValueToQDateTime();
};

void DomainClassesTest::init()
{
    QFile db{"testdb.db"};

    if (db.exists())
        QVERIFY(db.remove());
}

class WithQVectorReference;

class ReferencedByQVector : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id MEMBER m_id NOTIFY idChanged)
    Q_PROPERTY(WithQVectorReference* otherSide MEMBER m_otherSide NOTIFY otherSideChanged);

public:
    Q_INVOKABLE ReferencedByQVector() = default;

    int m_id{0};
    WithQVectorReference* m_otherSide{nullptr};

signals:
    void idChanged();
    void otherSideChanged();
};

class WithQVectorReference : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id MEMBER m_id NOTIFY idChanged)
    Q_PROPERTY(int dummy MEMBER m_dummy NOTIFY dummyChanged)
    Q_PROPERTY(
        QVector<ReferencedByQVector*> references MEMBER m_references NOTIFY referencesChanged)

public:
    Q_INVOKABLE WithQVectorReference() = default;

    int m_id{0};
    int m_dummy{42};
    QVector<ReferencedByQVector*> m_references;

signals:
    void idChanged();
    void referencesChanged();
    void dummyChanged();
};

void DomainClassesTest::testQVectorReference()
{
    qRegisterOrmEntity<WithQVectorReference, ReferencedByQVector>();

    {
        auto* parent = new WithQVectorReference;

        auto* ref1 = new ReferencedByQVector;
        ref1->m_otherSide = parent;

        auto* ref2 = new ReferencedByQVector;
        ref2->m_otherSide = parent;

        parent->m_references.push_back(ref1);
        parent->m_references.push_back(ref2);

        QOrmSession session;
        QVERIFY(session.merge(parent, ref1, ref2));
    }

    {
        QOrmSession session{QOrmSessionConfiguration::fromFile(":/qtorm_bypass_schema.json")};

        auto result = session.from<WithQVectorReference>().select().toVector();

        QCOMPARE(result.size(), 1);
        QCOMPARE(result.first()->m_references.size(), 2);
    }
}

class ReferencedByQSet;

class WithQSetReference : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id MEMBER m_id NOTIFY idChanged)
    Q_PROPERTY(int dummy MEMBER m_dummy NOTIFY dummyChanged)
    Q_PROPERTY(QSet<ReferencedByQSet*> references MEMBER m_references NOTIFY referencesChanged)

public:
    Q_INVOKABLE WithQSetReference() = default;

    int m_id{0};
    int m_dummy{42};
    QSet<ReferencedByQSet*> m_references;

signals:
    void idChanged();
    void referencesChanged();
    void dummyChanged();
};

class ReferencedByQSet : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id MEMBER m_id NOTIFY idChanged)
    Q_PROPERTY(WithQSetReference* otherSide MEMBER m_otherSide NOTIFY otherSideChanged);

public:
    Q_INVOKABLE ReferencedByQSet() = default;

    int m_id{0};
    WithQSetReference* m_otherSide{nullptr};

signals:
    void idChanged();
    void otherSideChanged();
};

void DomainClassesTest::testQSetReference()
{
    QSKIP("QSet<T*> reference is not implemented");

    qRegisterOrmEntity<WithQSetReference, ReferencedByQSet>();

    {
        auto* parent = new WithQSetReference;

        auto* ref1 = new ReferencedByQSet;
        ref1->m_otherSide = parent;

        auto* ref2 = new ReferencedByQSet;
        ref2->m_otherSide = parent;

        parent->m_references.insert(ref1);
        parent->m_references.insert(ref2);

        QOrmSession session;
        QVERIFY(session.merge(parent, ref1, ref2));
    }

    {
        QOrmSession session{QOrmSessionConfiguration::fromFile(":/qtorm_bypass_schema.json")};

        auto result = session.from<WithQSetReference>().select().toVector();

        QCOMPARE(result.size(), 1);
        QCOMPARE(result.first()->m_references.size(), 2);
    }
}

class WithStdVectorReference;

class ReferencedByStdVector : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id MEMBER m_id NOTIFY idChanged)
    Q_PROPERTY(WithStdVectorReference* otherSide MEMBER m_otherSide NOTIFY otherSideChanged);

public:
    Q_INVOKABLE ReferencedByStdVector() = default;

    int m_id{0};
    WithStdVectorReference* m_otherSide{nullptr};

signals:
    void idChanged();
    void otherSideChanged();
};

class WithStdVectorReference : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id MEMBER m_id NOTIFY idChanged)
    Q_PROPERTY(int dummy MEMBER m_dummy NOTIFY dummyChanged)
    Q_PROPERTY(QSet<ReferencedByStdVector*> references MEMBER m_references NOTIFY referencesChanged)

public:
    Q_INVOKABLE WithStdVectorReference() = default;

    int m_id{0};
    int m_dummy{42};
    QSet<ReferencedByStdVector*> m_references;

signals:
    void idChanged();
    void referencesChanged();
    void dummyChanged();
};

void DomainClassesTest::testStdVectorReference()
{
    QSKIP("std::vector<T*> reference is not implemented");

    qRegisterOrmEntity<WithStdVectorReference, ReferencedByStdVector>();

    {
        auto* parent = new WithStdVectorReference;

        auto* ref1 = new ReferencedByStdVector;
        ref1->m_otherSide = parent;

        auto* ref2 = new ReferencedByStdVector;
        ref2->m_otherSide = parent;

        parent->m_references.insert(ref1);
        parent->m_references.insert(ref2);

        QOrmSession session;
        QVERIFY(session.merge(parent, ref1, ref2));
    }

    {
        QOrmSession session{QOrmSessionConfiguration::fromFile(":/qtorm_bypass_schema.json")};

        auto result = session.from<WithStdVectorReference>().select().toVector();

        QCOMPARE(result.size(), 1);
        QCOMPARE(result.first()->m_references.size(), 2);
    }
}

class WithQDateTime : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id MEMBER m_id NOTIFY idChanged)
    Q_PROPERTY(QDateTime date MEMBER m_date NOTIFY dateChanged)

public:
    Q_INVOKABLE WithQDateTime() = default;

    int m_id{0};
    QDateTime m_date{QDateTime::currentDateTime()};

signals:
    void idChanged();
    void dateChanged();
};

void DomainClassesTest::testNullValueToQDateTime()
{
    qRegisterOrmEntity<WithQDateTime>();

    {
        auto entity = new WithQDateTime;
        entity->m_date = QDateTime{};
        QOrmSession session;
        QVERIFY(session.merge(entity));
    }

    {
        QOrmSession session{QOrmSessionConfiguration::fromFile(":/qtorm_bypass_schema.json")};
        auto result = session.from<WithQDateTime>().select().toVector();
        QCOMPARE(result.size(), 1);
        QVERIFY(result.first()->m_date.isNull());
    }
}

QTEST_GUILESS_MAIN(DomainClassesTest)

#include "tst_domainclasses.moc"
