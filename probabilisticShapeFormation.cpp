#include "probabilisticShapeFormation.h"
#include "buzz/buzzvm.h"

static CRange<Real> STIMULUS_RANGE(0.0, 5000.0);

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
      /* Make sure its value is correct */
      if(nGoal >= m_vecTaskCounts.size()) {
         LOGERR << str_robot_id << ": variable 'target_goal' has wrong value " << nGoal << std::endl;
         return;
      }
      ++m_vecTaskCounts[nGoal];
      /* Set the mapping */
      m_vecRobotsTasks[t_vm->robot] = nGoal;
      /* Get the current thresholds */
      BuzzTableOpen(t_vm, "positional_error");
      buzzobj_t tPositionalError = BuzzGet(t_vm, "positional_error");
      /* Make sure it's the type we expect (a table) */
      if(!buzzobj_isfloat(tPositionalError)) {
         LOGERR << str_robot_id << ": variable 'positional_error' has wrong type " << buzztype_desc[tPositionalError->o.type] << std::endl;
         return;
      }
      /* Get the values */
      float nPositionError = buzzobj_getfloat(tPositionalError);
      /* Make sure its value is correct */
      if(nPositionError >= m_vecTaskCounts.size()) {
         LOGERR << str_robot_id << ": variable 'positional_error' has wrong value " << nPositionError << std::endl;
         return;
      }
      ++m_vecTaskCounts[nPositionError];
      /* Set the mapping */
      m_vecRobotsTasks[t_vm->robot] = nPositionError;
   }

   /** Task counter */
   std::vector<int> m_vecTaskCounts;
   /* Task-robot mapping */
   std::map<int,int> m_vecRobotsTasks;
   /* Robot-threshold mapping */
   std::map<int,std::vector<float> > m_vecRobotsProbabilistic;
};

/****************************************/
/****************************************/

/**
 * Functor to put the stimulus in the Buzz VMs.
 */
struct PutStimuli : public CBuzzLoopFunctions::COperation {

   /** Constructor */
   PutStimuli(const std::vector<Real>& vec_stimuli) : m_vecStimuli(vec_stimuli) {}
   
   /** The action happens here */
   virtual void operator()(const std::string& str_robot_id,
                           buzzvm_t t_vm) {
      /* Set the values of the table 'stimulus' in the Buzz VM */
      BuzzTableOpen(t_vm, "stimulus");
      for(int i = 0; i < m_vecStimuli.size(); ++i) {
         BuzzTablePut(t_vm, i, static_cast<float>(m_vecStimuli[i]));
      }
      BuzzTableClose(t_vm);
   }

   /** Calculated stimuli */
   const std::vector<Real>& m_vecStimuli;
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
   GetRobotData cGetRobotData(m_vecStimuli.size());
   BuzzForeachVM(cGetRobotData);
   /* Flush data to the output file */
   if(!cGetRobotData.m_vecRobotsProbabilistic.empty()) {
      for(int i = 0; i < GetNumRobots(); ++i) {
         m_cOutFile << GetSpace().GetSimulationClock() << "\t"
                    << i << "\t"
                    << cGetRobotData.m_vecRobotsTasks[i];
         for(int j = 0; j < m_vecStimuli.size(); ++j) {
            m_cOutFile << "\t" << cGetRobotData.m_vecRobotsProbabilistic[i][j];
         }
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

void CProbabilisticShapeFormation::BuzzBytecodeUpdated() {
   /* Convey the stimuli to every robot */
   BuzzForeachVM(PutStimuli(m_vecStimuli));
}

/****************************************/
/****************************************/

REGISTER_LOOP_FUNCTIONS(CProbabilisticShapeFormation, "probabilisticShapeFormation");
