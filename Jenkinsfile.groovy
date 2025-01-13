def UUID_DIR  = UUID.randomUUID().toString()
def url = "https://jenkins.snow1k.com/job/MedivhEmu/${BUILD_NUMBER}/"

pipeline {

    agent any

    stages {
        stage('Build') {
            steps {
                echo 'Hello World2'
            }
        }
    }

    post {
        always {
            script {
                String emoj = getEmoj(currentBuild.result)
                rocketSend channel: '#wowemu', message: "MedivhEmu - BUILD-STATUS : ${currentBuild.result}. Report: ${url}", emoji: emoj, rawMessage: true
            }
        }
    }

}

def String getEmoj(String status) {
    switch(status) {
        case 'SUCCESS': return ':smirk:'
        case 'FAILURE': return ':sob:'
        case 'UNSTABLE': return ':thinking:'
        default:
            return ':sob:'
    }
}