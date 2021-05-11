#include "probabilisticShapeFormation.h"
#include "buzz/buzzvm.h"

/****************************************/
/****************************************/

/**
 * Functor to get data from the robots
 */
struct GetRobotData : public CBuzzLoopFunctions::COperation {

   /** Constructor */
  GetRobotData(int n_tasks) : m_vecTaskCounts(n_tasks, 0) {}

   /** The action happens here */
   virtual void operator()(const std::string& str_robot_id,
                           buzzvm_t t_vm) {
      /* Get the current task */
      buzzobj_t tGoal = BuzzGet(t_vm, "target_goal");
      /* Make sure it's the type we expect (an integer) */
      if(!buzzobj_isint(tGoal)) {
         LOGERR << str_robot_id << ": variable 'target_goal' has wrong type " << buzztype_desc[tGoal->o.type] << std::endl;
         return;
      }
      /* Get the value */
      int nGoal = buzzobj_getint(tGoal);
      ++m_vecRobotsGoals[nGoal];
      /* Set the mapping */
      m_vecRobotsGoals[t_vm->robot] = nGoal;
      /* Get the current error */
      buzzobj_t tPositionalError = BuzzGet(t_vm, "positional_error");
      if(!buzzobj_isfloat(tPositionalError)) {
         LOGERR << str_robot_id << ": variable 'positional_error' has wrong type " << buzztype_desc[tPositionalError->o.type] << std::endl;
         return;
      }
      /* Get the values */
      float nPositionError = buzzobj_getfloat(tPositionalError);
      ++m_vecRobotsError[nPositionError];
      /* Set the mapping */
      m_vecRobotsError[t_vm->robot] = nPositionError;
   }
   std::vector<int> m_vecTaskCounts;
   /* Goal-robot mapping */
   std::map<int,int> m_vecRobotsGoals;
   /* Robot-error mapping */
   std::map<int,float > m_vecRobotsError;
};


/****************************************/
/****************************************/

void CProbabilisticShapeFormation::Init(TConfigurationNode& t_tree) {
   /* Call parent Init() */
   CBuzzLoopFunctions::Init(t_tree);
   /* Parse XML tree */
   GetNodeAttribute(t_tree, "outfile", m_strOutFile);
   /* Create a new RNG */
   m_pcRNG = CRandom::CreateRNG("argos");
   /* Open the output file */
   m_cOutFile.open(m_strOutFile.c_str(),
                   std::ofstream::out | std::ofstream::trunc);
}

/****************************************/
/****************************************/

void CProbabilisticShapeFormation::Reset() {
   /* Reset the output file */
   m_cOutFile.open(m_strOutFile.c_str(),
                   std::ofstream::out | std::ofstream::trunc);

}

/****************************************/
/****************************************/

void CProbabilisticShapeFormation::Destroy() {
   m_cOutFile.close();
}

/****************************************/
/****************************************/

void CProbabilisticShapeFormation::PostStep() {
   /* Get robot data */
   GetRobotData cGetRobotData(0);;
   BuzzForeachVM(cGetRobotData);
   /* Flush data to the output file */
   if(!cGetRobotData.m_vecRobotsError.empty()) {
      for(int i = 0; i < GetNumRobots(); ++i) {
         m_cOutFile << GetSpace().GetSimulationClock() << "\t"
                    << i << "\t"
                    << cGetRobotData.m_vecRobotsGoals[i] << "\t"
                    <<cGetRobotData.m_vecRobotsError[i];
         m_cOutFile << std::endl;
      }
   }
}

/****************************************/
/****************************************/

bool CProbabilisticShapeFormation::IsExperimentFinished() {
   /* Feel free to try out custom ending conditions */
   return false;
}

/****************************************/
/****************************************/

int CProbabilisticShapeFormation::GetNumRobots() const {
   return m_mapBuzzVMs.size();
}

/****************************************/
/****************************************/

REGISTER_LOOP_FUNCTIONS(CProbabilisticShapeFormation, "probabilisticShapeFormation");
