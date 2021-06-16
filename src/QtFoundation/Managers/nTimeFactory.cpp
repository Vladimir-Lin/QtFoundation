#include <qtfoundation.h>

N::TimeFactory:: TimeFactory ( PurePlan * p )
               : plan        (            p )
{
}

N::TimeFactory::~TimeFactory(void)
{
}

N::StarDate * N::TimeFactory::create(int type)
{
  switch (type)                                  {
    case Calendars::StarDate                     :
    return new StarDate                    ( )   ;
    case Calendars::StarTime                     :
    return new Time                        ( )   ;
    case Calendars::ISO                          :
    return new IsoDateTime                 ( )   ;
    case Calendars::Cosmos                       :
    return new CosmosCalendar              ( )   ;
    case Calendars::Geogrian                     :
    return new DateTime                    ( )   ;
    case Calendars::Proleptic                    :
    return new ProlepticGregorian          ( )   ;
    case Calendars::Julian                       :
    return new JulianCalendar              ( )   ;
    case Calendars::Holocene                     :
    return new HoloceneCalendar            ( )   ;
    case Calendars::Buddhist                     :
    return new BuddhistCalendar            ( )   ;
    case Calendars::Taiwanese                    :
    return new TaiwanCalendar              ( )   ;
    case Calendars::Juche                        :
    return new JucheCalendar               ( )   ;
    case Calendars::Bengali                      :
    return new BengaliCalendar             ( )   ;
    case Calendars::Lunar                        :
    return new LunarCalendar               ( )   ;
    case Calendars::World                        :
    return new WorldCalendar               ( )   ;
    case Calendars::Bahai                        :
    return new BahaiCalendar               ( )   ;
    case Calendars::Armenian                     :
    return new ArmenianCalendar            ( )   ;
    case Calendars::Zoroastrian                  :
    return new ZoroastrianCalendar         ( )   ;
    case Calendars::Berber                       :
    return new BerberCalendar              ( )   ;
    case Calendars::Burmese                      :
    return new BurmeseCalendar             ( )   ;
    case Calendars::Byzantine                    :
    return new ByzantineCalendar           ( )   ;
    case Calendars::Coptic                       :
    return new CopticCalendar              ( )   ;
    case Calendars::Ethiopian                    :
    return new EthiopianCalendar           ( )   ;
    case Calendars::Hebrew                       :
    return new HebrewCalendar              ( )   ;
    case Calendars::Hindu                        :
    return new HinduCalendar               ( )   ;
    case Calendars::Persian                      :
    return new PersianCalendar             ( )   ;
    case Calendars::Islamic                      :
    return new IslamicCalendar             ( )   ;
    case Calendars::Thai                         :
    return new ThaiCalendar                ( )   ;
    case Calendars::Assyrian                     :
    return new AssyrianCalendar            ( )   ;
    case Calendars::Perpetual                    :
    return new PerpetualCalendar           ( )   ;
    case Calendars::Akan                         :
    return new AkanCalendar                ( )   ;
    case Calendars::Discordian                   :
    return new DiscordianCalendar          ( )   ;
    case Calendars::Germanic                     :
    return new GermanicCalendar            ( )   ;
    case Calendars::Igbo                         :
    return new IgboCalendar                ( )   ;
    case Calendars::Gaelic                       :
    return new GaelicCalendar              ( )   ;
    case Calendars::Jain                         :
    return new JainCalendar                ( )   ;
    case Calendars::Saka                         :
    return new SakaCalendar                ( )   ;
    case Calendars::Javanese                     :
    return new JavaneseCalendar            ( )   ;
    case Calendars::Kurdish                      :
    return new KurdishCalendar             ( )   ;
    case Calendars::Lithuanian                   :
    return new LithuanianCalendar          ( )   ;
    case Calendars::Malayalam                    :
    return new MalayalamCalendar           ( )   ;
    case Calendars::Nanakshahi                   :
    return new NanakshahiCalendar          ( )   ;
    case Calendars::Nepali                       :
    return new NepaliCalendar              ( )   ;
    case Calendars::NepalSambat                  :
    return new NepalSambatCalendar         ( )   ;
    case Calendars::RevisedJulian                :
    return new RevisedJulianCalendar       ( )   ;
    case Calendars::Romanian                     :
    return new RomanianCalendar            ( )   ;
    case Calendars::Runic                        :
    return new RunicCalendar               ( )   ;
    case Calendars::Tamil                        :
    return new TamilCalendar               ( )   ;
    case Calendars::ThaiLunar                    :
    return new ThaiLunarCalendar           ( )   ;
    case Calendars::Tibetan                      :
    return new TibetanCalendar             ( )   ;
    case Calendars::Xhosa                        :
    return new XhosaCalendar               ( )   ;
    case Calendars::Yoruba                       :
    return new YorubaCalendar              ( )   ;
    case Calendars::Attic                        :
    return new AtticCalendar               ( )   ;
    case Calendars::Aztec                        :
    return new AztecCalendar               ( )   ;
    case Calendars::Babylonian                   :
    return new BabylonianCalendar          ( )   ;
    case Calendars::Bulgar                       :
    return new BulgarCalendar              ( )   ;
    case Calendars::Coligny                      :
    return new ColignyCalendar             ( )   ;
    case Calendars::Egyptian                     :
    return new EgyptianCalendar            ( )   ;
    case Calendars::Enoch                        :
    return new EnochCalendar               ( )   ;
    case Calendars::Florentine                   :
    return new FlorentineCalendar          ( )   ;
    case Calendars::FrenchRepublican             :
    return new FrenchRepublicanCalendar    ( )   ;
    case Calendars::Hellenic                     :
    return new HellenicCalendar            ( )   ;
    case Calendars::OldIcelandic                 :
    return new OldIcelandicCalendar        ( )   ;
    case Calendars::AncientMacedonian            :
    return new AncientMacedonianCalendar   ( )   ;
    case Calendars::Mesoamerican                 :
    return new MesoamericanCalendar        ( )   ;
    case Calendars::Pentecontad                  :
    return new PentecontadCalendar         ( )   ;
    case Calendars::Positivist                   :
    return new PositivistCalendar          ( )   ;
    case Calendars::RapaNui                      :
    return new RapaNuiCalendar             ( )   ;
    case Calendars::Roman                        :
    return new RomanCalendar               ( )   ;
    case Calendars::Rumi                         :
    return new RumiCalendar                ( )   ;
    case Calendars::Soviet                       :
    return new SovietCalendar              ( )   ;
    case Calendars::Swedish                      :
    return new SwedishCalendar             ( )   ;
    case Calendars::Celtic                       :
    return new CelticCalendar              ( )   ;
    case Calendars::Inca                         :
    return new IncaCalendar                ( )   ;
    case Calendars::Turkmen                      :
    return new TurkmenCalendar             ( )   ;
    case Calendars::ProlepticJulian              :
    return new ProlepticJulianCalendar     ( )   ;
    case Calendars::Dreamspell                   :
    return new DreamspellCalendar          ( )   ;
    case Calendars::Invariable                   :
    return new InvariableCalendar          ( )   ;
    case Calendars::Tranquility                  :
    return new TranquilityCalendar         ( )   ;
    case Calendars::BalinesePawukon              :
    return new BalinesePawukonCalendar     ( )   ;
    case Calendars::BalineseSaka                 :
    return new BalineseSakaCalendar        ( )   ;
    case Calendars::Pax                          :
    return new PaxCalendar                 ( )   ;
    case Calendars::CommonCivilTime              :
    return new CommonCivilTimeCalendar     ( )   ;
    case Calendars::Symmetry                     :
    return new SymmetryCalendar            ( )   ;
    case Calendars::HankeHenryPermanent          :
    return new HankeHenryPermanentCalendar ( )   ;
    case Calendars::HermeticLeapWeek             :
    return new HermeticLeapWeekCalendar    ( )   ;
    case Calendars::Darian                       :
    return new DarianCalendar              ( )   ;
    case Calendars::MiddleEarth                  :
    return new MiddleEarthCalendar         ( )   ;
    case Calendars::GalaxyLegend                 :
    return new GalaxyLegendCalendar        ( )   ;
    case Calendars::GalaxyRepublic               :
    return new GalaxyRepublicCalendar      ( )   ;
    case Calendars::GalaxyEmperor                :
    return new GalaxyEmperorCalendar       ( )   ;
    case Calendars::NewGalaxyEmperor             :
    return new NewGalaxyEmperorCalendar    ( )   ;
    case Calendars::Regnal                       :
    return new RegnalCalendar              ( )   ;
    case Calendars::AbUrbeCondita                :
    return new AbUrbeConditaCalendar       ( )   ;
    case Calendars::Japanese                     :
    return new JapanCalendar               ( )   ;
    case Calendars::Korean                       :
    return new KoreanCalendar              ( )   ;
    case Calendars::Confucius                    :
    return new ConfuciusCalendar           ( )   ;
    case Calendars::Huangdi                      :
    return new HuangdiCalendar             ( )   ;
    case Calendars::Dao                          :
    return new DaoCalendar                 ( )   ;
    case Calendars::Dai                          :
    return new DaiCalendar                 ( )   ;
    case Calendars::Maya                         :
    return new MayaCalendar                ( )   ;
    case Calendars::Tzolkin                      :
    return new TzolkinCalendar             ( )   ;
    case Calendars::Haab                         :
    return new HaabCalendar                ( )   ;
    case Calendars::MayaLunar                    :
    return new MayaLunarCalendar           ( )   ;
    case Calendars::MayaVenus                    :
    return new MayaVenusCalendar           ( )   ;
    case Calendars::India                        :
    return new IndiaCalendar               ( )   ;
    case Calendars::StarTrek                     :
    return new StarTrek                    ( )   ;
  }                                              ;
  return NULL                                    ;
}

bool N::TimeFactory::load(void)
{
  UUIDs     Uuids                     ;
  toUuids . clear ( )                 ;
  toTypes . clear ( )                 ;
  Names   . clear ( )                 ;
  SqlConnection SC(plan->sql)         ;
  if (SC.open("nTimeFactory"))        {
    QString Q                         ;
    SUID    uuid                      ;
    int     type                      ;
    Q = SC.sql.SelectFrom             (
          "uuid,type"                 ,
          PlanTable(Calendars)        ,
          "order by type asc")        ;
    SqlLoopNow ( SC , Q )             ;
      uuid = SC . Uuid ( 0 )          ;
      type = SC . Int  ( 1 )          ;
      Uuids << uuid                   ;
      toUuids[type] = uuid            ;
      toTypes[uuid] = type            ;
    SqlLoopErr ( SC , Q )             ;
    SqlLoopEnd ( SC , Q )             ;
    foreach (uuid,Uuids)              {
      type = toTypes[uuid]            ;
      Names[type] = SC.getName        (
        PlanTable(Names)              ,
        "uuid"                        ,
        plan->LanguageId              ,
        uuid                        ) ;
    }                                 ;
    SC.close()                        ;
  }                                   ;
  SC.remove()                         ;
  ///////////////////////////////////////////////////////////////////////
  Formats[Calendars::StarDate           ] = "%1"                        ;
  Formats[Calendars::StarTime           ] = "%1.%2"                     ;
  Formats[Calendars::ISO                ] = "yyyy/MM/dd hh:mm:ss"       ;
  Formats[Calendars::Cosmos             ] = "%1.%2.%3.%4.%5.%6"         ;
  Formats[Calendars::Geogrian           ] = "yyyy/MM/dd hh:mm:ss"       ;
  Formats[Calendars::Proleptic          ] = "yyyy/MM/dd hh:mm:ss"       ;
  Formats[Calendars::Julian             ] = "yyyy/MM/dd hh:mm:ss"       ;
  Formats[Calendars::Holocene           ] = "/nn/ yyyyy/MM/dd hh:mm:ss" ;
  Formats[Calendars::Buddhist           ] = "yyyy/MM/dd hh:mm:ss"       ;
  Formats[Calendars::Taiwanese          ] = "/nn/ yyy/MM/dd hh:mm:ss"   ;
  Formats[Calendars::Juche              ] = "/nn/ yyy/MM/dd hh:mm:ss"   ;
  Formats[Calendars::Bengali            ] = ""                          ;
  Formats[Calendars::Lunar              ] = ""                          ;
  Formats[Calendars::World              ] = "/nn/ yyyy/MM/dd hh:mm:ss"  ;
  Formats[Calendars::Bahai              ] = ""                          ;
  Formats[Calendars::Armenian           ] = ""                          ;
  Formats[Calendars::Zoroastrian        ] = ""                          ;
  Formats[Calendars::Berber             ] = ""                          ;
  Formats[Calendars::Burmese            ] = ""                          ;
  Formats[Calendars::Byzantine          ] = ""                          ;
  Formats[Calendars::Coptic             ] = ""                          ;
  Formats[Calendars::Ethiopian          ] = ""                          ;
  Formats[Calendars::Hebrew             ] = ""                          ;
  Formats[Calendars::Hindu              ] = ""                          ;
  Formats[Calendars::Persian            ] = ""                          ;
  Formats[Calendars::Islamic            ] = ""                          ;
  Formats[Calendars::Thai               ] = ""                          ;
  Formats[Calendars::Assyrian           ] = ""                          ;
  Formats[Calendars::Perpetual          ] = ""                          ;
  Formats[Calendars::Akan               ] = ""                          ;
  Formats[Calendars::Discordian         ] = ""                          ;
  Formats[Calendars::Germanic           ] = ""                          ;
  Formats[Calendars::Igbo               ] = ""                          ;
  Formats[Calendars::Gaelic             ] = ""                          ;
  Formats[Calendars::Jain               ] = ""                          ;
  Formats[Calendars::Saka               ] = ""                          ;
  Formats[Calendars::Javanese           ] = ""                          ;
  Formats[Calendars::Kurdish            ] = ""                          ;
  Formats[Calendars::Lithuanian         ] = ""                          ;
  Formats[Calendars::Malayalam          ] = ""                          ;
  Formats[Calendars::Nanakshahi         ] = ""                          ;
  Formats[Calendars::Nepali             ] = ""                          ;
  Formats[Calendars::NepalSambat        ] = ""                          ;
  Formats[Calendars::RevisedJulian      ] = ""                          ;
  Formats[Calendars::Romanian           ] = ""                          ;
  Formats[Calendars::Runic              ] = ""                          ;
  Formats[Calendars::Tamil              ] = ""                          ;
  Formats[Calendars::ThaiLunar          ] = ""                          ;
  Formats[Calendars::Tibetan            ] = ""                          ;
  Formats[Calendars::Xhosa              ] = ""                          ;
  Formats[Calendars::Yoruba             ] = ""                          ;
  Formats[Calendars::Attic              ] = ""                          ;
  Formats[Calendars::Aztec              ] = ""                          ;
  Formats[Calendars::Babylonian         ] = ""                          ;
  Formats[Calendars::Bulgar             ] = ""                          ;
  Formats[Calendars::Coligny            ] = ""                          ;
  Formats[Calendars::Egyptian           ] = ""                          ;
  Formats[Calendars::Enoch              ] = ""                          ;
  Formats[Calendars::Florentine         ] = ""                          ;
  Formats[Calendars::FrenchRepublican   ] = ""                          ;
  Formats[Calendars::Hellenic           ] = ""                          ;
  Formats[Calendars::OldIcelandic       ] = ""                          ;
  Formats[Calendars::AncientMacedonian  ] = ""                          ;
  Formats[Calendars::Mesoamerican       ] = ""                          ;
  Formats[Calendars::Pentecontad        ] = ""                          ;
  Formats[Calendars::Positivist         ] = ""                          ;
  Formats[Calendars::RapaNui            ] = ""                          ;
  Formats[Calendars::Roman              ] = ""                          ;
  Formats[Calendars::Rumi               ] = ""                          ;
  Formats[Calendars::Soviet             ] = ""                          ;
  Formats[Calendars::Swedish            ] = ""                          ;
  Formats[Calendars::Celtic             ] = ""                          ;
  Formats[Calendars::Inca               ] = ""                          ;
  Formats[Calendars::Turkmen            ] = ""                          ;
  Formats[Calendars::ProlepticJulian    ] = ""                          ;
  Formats[Calendars::Dreamspell         ] = ""                          ;
  Formats[Calendars::Invariable         ] = ""                          ;
  Formats[Calendars::Tranquility        ] = ""                          ;
  Formats[Calendars::BalinesePawukon    ] = ""                          ;
  Formats[Calendars::BalineseSaka       ] = ""                          ;
  Formats[Calendars::Pax                ] = ""                          ;
  Formats[Calendars::CommonCivilTime    ] = ""                          ;
  Formats[Calendars::Symmetry           ] = ""                          ;
  Formats[Calendars::HankeHenryPermanent] = ""                          ;
  Formats[Calendars::HermeticLeapWeek   ] = ""                          ;
  Formats[Calendars::Darian             ] = ""                          ;
  Formats[Calendars::MiddleEarth        ] = ""                          ;
  Formats[Calendars::GalaxyLegend       ] = "/nn/ yyy/MM/dd hh:mm:ss"   ;
  Formats[Calendars::GalaxyRepublic     ] = "/nn/ yyy/MM/dd hh:mm:ss"   ;
  Formats[Calendars::GalaxyEmperor      ] = "/nn/ yyy/MM/dd hh:mm:ss"   ;
  Formats[Calendars::NewGalaxyEmperor   ] = "/nn/ yyy/MM/dd hh:mm:ss"   ;
  Formats[Calendars::Regnal             ] = ""                          ;
  Formats[Calendars::AbUrbeCondita      ] = ""                          ;
  Formats[Calendars::Japanese           ] = ""                          ;
  Formats[Calendars::Korean             ] = ""                          ;
  Formats[Calendars::Confucius          ] = "/nn/ yyyy/MM/dd hh:mm:ss"  ;
  Formats[Calendars::Huangdi            ] = "/nn/ yyyy/MM/dd hh:mm:ss"  ;
  Formats[Calendars::Dao                ] = ""                          ;
  Formats[Calendars::Dai                ] = ""                          ;
  Formats[Calendars::Maya               ] = ""                          ;
  Formats[Calendars::Tzolkin            ] = ""                          ;
  Formats[Calendars::Haab               ] = ""                          ;
  Formats[Calendars::MayaLunar          ] = ""                          ;
  Formats[Calendars::MayaVenus          ] = ""                          ;
  Formats[Calendars::India              ] = ""                          ;
  Formats[Calendars::StarTrek           ] = "/nn/ %1.%2"                ;
  return true                                                           ;
}
